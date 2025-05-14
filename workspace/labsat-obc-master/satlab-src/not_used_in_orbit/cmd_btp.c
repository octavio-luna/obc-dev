/*
 * Copyright (c) 2013-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/* Get the GNU version of basename(3) */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <csp/csp.h>
#include <csp/arch/csp_time.h>

// #include <util/util.h>
// #include <slash/slash.h>

#include <btp/client.h>
#include <btp/error.h>
#include <btp/types.h>
#include <btp/cmd_btp.h>

#define DEFAULT_BACKEND		"file"
#define DEFAULT_TIMEOUT		1000
#define DEFAULT_TIMEOUT_CSUM	10000
#define DEFAULT_BSIZE		186
#define DEFAULT_COUNT		25
#define DEFAULT_ATTEMPTS	3

#define PROGRESS_WIDTH		40U

struct progress_state {
	int sizechars;
	uint32_t last;
	uint32_t lt;
	uint32_t bps;
	uint32_t last_print;
	uint32_t last_print_speed;
	uint32_t remain;
	bool quiet;
	struct slash *slash;
	bool abort;
};


static char *pretty_bytes(uint32_t bps)
{
	static char buf[25];

	if (bps >= 1048576)
		snprintf(buf, sizeof(buf), "%.1f Mbps", bps/1048576.0);
	else if (bps >= 1024)
		snprintf(buf, sizeof(buf), "%.1f kbps", bps/1024.0);
	else
		snprintf(buf, sizeof(buf), "%u bps", bps);

	return buf;
}

static char *pretty_seconds(uint32_t seconds)
{
	static char buf[25];
	uint32_t hrs = 0, min = 0, sec = 0;

	hrs = seconds / 3600;
	min = (seconds - hrs*3600) / 60;
	sec = seconds - hrs*3600 - min*60;

	if (seconds >= 3600)
		snprintf(buf, sizeof(buf), "%uh %um %us", hrs, min, sec);
	else if (seconds >= 60)
		snprintf(buf, sizeof(buf), "%um %us", min, sec);
	else
		snprintf(buf, sizeof(buf), "%us", sec);

	return buf;
}


int cmd_btp_push(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c;
	char *endptr;

	struct btp_context *btp;
	bool force = false;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t timeout_csum = DEFAULT_TIMEOUT_CSUM;
	uint32_t timeout_server = 0;
	uint32_t options = CSP_O_NONE;
	uint32_t bsize = DEFAULT_BSIZE;
	uint32_t count = DEFAULT_COUNT;
	unsigned int attempts = DEFAULT_ATTEMPTS;
	uint32_t start, stop;

	char *backend = (char *)DEFAULT_BACKEND;
	char *remotepath = NULL;
	char *localpath = NULL;
	char pathbuf[BLOB_NAME_SIZE];

	struct progress_state pgstate = {};
	pgstate.slash = slash;

	/* Parse command line arguments */
	while ((c = slash_getopt(slash, "p:b:s:c:t:T:i:a:qrfh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'b':
			backend = slash->optarg;
			break;
		case 's':
			bsize = atoi(slash->optarg);
			break;
		case 'c':
			count = atoi(slash->optarg);
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'T':
			timeout_server = atoi(slash->optarg);
			break;
		case 'i':
			timeout_csum = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'q':
			pgstate.quiet = true;
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'f':
			force = true;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	if (remain != 2)
		return SLASH_EUSAGE;

	remotepath = slash->argv[index + 1];
	if (remotepath[strlen(remotepath) - 1] == '/') {
		strncpy(pathbuf, remotepath, BLOB_NAME_SIZE - 1);
		pathbuf[BLOB_NAME_SIZE - 1] = '\0';
		strncat(pathbuf, basename(slash->argv[index]), BLOB_NAME_SIZE - 1);
		pathbuf[BLOB_NAME_SIZE - 1] = '\0';
		remotepath = pathbuf;
	}
	localpath = slash->argv[index];

	/* Default server side timeout 10x client */
	if (!timeout_server)
		timeout_server = 10 * timeout;

	/* Connect to server */
	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	/* Establish connection to server */
	ret = btp_client_upload(btp, backend, remotepath, localpath, bsize, timeout_csum, timeout_server, force);
	if (ret != BTP_EOK) {
		UPLOG_ERR("Could not push %s: %s\n", localpath, btp_error(ret));
		goto complete;
	}

	/* Early exit if transfer is already complete */
	if (btp_client_finished(btp)) {
		UPLOG_NOTICE("File already complete\n");
		goto complete;
	}

	/* Log start time for statistics */
	start = csp_get_ms();

	/* Get initial status */
	ret = btp_client_request_status(btp);
	if (ret != BTP_EOK) {
		UPLOG_ERR("\nCould not get status: %s\n", btp_error(ret));
		goto disconnect;
	} else if (pgstate.abort) {
		goto disconnect;
	}

	/* Send blocks until completion or error */
	while (!btp_client_finished(btp)) {
		pretty_progress(btp, &pgstate);

		/* Send blocks */
		ret = btp_client_send_blocks(btp, BTP_OFFSET_NEXT, count, pretty_progress, &pgstate);
		if (ret != BTP_EOK) {
			UPLOG_ERR("\nCould not send blocks: %s\n", btp_error(ret));
			goto disconnect;
		} else if (pgstate.abort) {
			goto complete;
		}

		/* Send status request */
		ret = btp_client_request_status(btp);
		if (ret != BTP_EOK) {
			UPLOG_ERR("\nCould not get status: %s\n", btp_error(ret));
			goto disconnect;
		} else if (pgstate.abort) {
			goto complete;
		}
	}

	/* Log transfer speed */
	if (!pgstate.quiet && !pgstate.abort) {
		stop = csp_get_ms();
		pretty_progress(btp, &pgstate);
		UPDEBUG("\n%s/s (%"PRIu32" bytes in %.3fs)\n",
		       pretty_bytes((btp->progress * 8) / ((stop - start) / 1000.0)),
		       btp->progress, (stop - start) / 1000.0);
	}

complete:
	/* Complete transfer */
	ret = btp_client_complete(btp, timeout_csum);
	if (ret == BTP_ESTALE)
		UPLOG_ERR("WARNING: checksum mismatch!\n");

disconnect:
	/* Close connection */
	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}

int cmd_btp_pull(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c;
	char *endptr;

	struct btp_context *btp;
	bool force = false;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t timeout_csum = DEFAULT_TIMEOUT_CSUM;
	uint32_t timeout_server = 0;
	uint32_t options = CSP_O_NONE;
	uint32_t bsize = DEFAULT_BSIZE;
	uint32_t count = DEFAULT_COUNT;
	unsigned int attempts = DEFAULT_ATTEMPTS;
	uint32_t start, stop;

	char *backend = (char *)DEFAULT_BACKEND;
	char *remotepath = NULL;
	char *localpath = NULL;

	struct progress_state pgstate = {};
	pgstate.slash = slash;

	/* Parse command line arguments */
	while ((c = slash_getopt(slash, "p:b:s:c:t:T:i:a:qrfh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'b':
			backend = slash->optarg;
			break;
		case 's':
			bsize = atoi(slash->optarg);
			break;
		case 'c':
			count = atoi(slash->optarg);
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'i':
			timeout_csum = atoi(slash->optarg);
			break;
		case 'T':
			timeout_server = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'q':
			pgstate.quiet = true;
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'f':
			force = true;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	if (remain == 2) {
		localpath = slash->argv[index + 1];
	} else if (remain == 1) {
		localpath = basename(slash->argv[index]);
	} else {
		return SLASH_EUSAGE;
	}
	remotepath = slash->argv[index];

	/* Default server side timeout 10x client */
	if (!timeout_server)
		timeout_server = 10 * timeout;

	/* Connect to server */
	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	/* Establish connection to server */
	ret = btp_client_download(btp, backend, remotepath, localpath, bsize, timeout_csum, timeout_server, force);
	if (ret != BTP_EOK) {
		UPLOG_ERR("Could not download %s: %s\n", remotepath, btp_error(ret));
		goto disconnect;
	}

	/* Mark transfer done if requested file is empty */
	if (btp->block_status.blocks == 0)
		btp->block_status.complete = true;

	/* Early exit if transfer is already complete */
	if (btp_client_finished(btp)) {
		UPLOG_NOTICE("File already complete\n");
		goto complete;
	}

	/* Log start time for statistics */
	start = csp_get_ms();

	/* Request blocks while file is incomplete */
	while (!btp_client_finished(btp)) {
		ret = btp_client_send_status(btp);
		if (ret != BTP_EOK) {
			UPLOG_ERR("\nCould not send status: %s\n", btp_error(ret));
			goto disconnect;
		} else if (pgstate.abort) {
			goto complete;
		}

		pretty_progress(btp, &pgstate);

		ret = btp_client_get_blocks(btp, BTP_OFFSET_NEXT, count, pretty_progress, &pgstate);
		if (ret != BTP_EOK) {
			UPLOG_ERR("\nCould not get blocks: %s\n", btp_error(ret));
			goto disconnect;
		} else if (pgstate.abort) {
			goto complete;
		}
	}

	/* Log transfer speed */
	if (!pgstate.quiet && !pgstate.abort) {
		stop = csp_get_ms();
		pretty_progress(btp, &pgstate);
		UPDEBUG("\n%s/s (%"PRIu32" bytes in %.3fs)\n",
		       pretty_bytes((btp->progress * 8) / ((stop - start) / 1000.0)),
		       btp->progress, (stop - start) / 1000.0);
	}

complete:
	/* Complete transfer */
	ret = btp_client_complete(btp, timeout_csum);
	if (ret == BTP_ESTALE) {
		UPLOG_WARNING("WARNING: checksum mismatch!\n");
	} else if (ret != BTP_EOK) {
		UPLOG_ERR("Could not complete %s: %s\n", remotepath, btp_error(ret));
	}

disconnect:
	/* Close connection */
	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}

int cmd_btp_list(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c;
	char *endptr;

	struct btp_context *btp;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t options = CSP_O_NONE;
	uint32_t count = DEFAULT_COUNT;
	unsigned int attempts = DEFAULT_ATTEMPTS;

	char *backend = (char *)DEFAULT_BACKEND;
	char *remotepath = NULL;

	/* Parse command line arguments */
	while ((c = slash_getopt(slash, "p:b:c:t:a:rh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'b':
			backend = slash->optarg;
			break;
		case 'c':
			count = atoi(slash->optarg);
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	if (remain != 1)
		return SLASH_EUSAGE;

	remotepath = slash->argv[index];

	/* Connect to server */
	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	ret = btp_client_list(btp, count, backend, remotepath, btp_list_print_cb, NULL);
	if (ret != BTP_EOK)
		UPLOG_ERR("Could not list %s: %s\n", remotepath, btp_error(ret));

	/* Close connection */
	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}

int cmd_btp_remove(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c;
	char *endptr;

	struct btp_context *btp;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t options = CSP_O_NONE;
	unsigned int attempts = DEFAULT_ATTEMPTS;

	char *backend = (char *)DEFAULT_BACKEND;
	char *remotepath = NULL;

	/* Parse command line arguments */
	while ((c = slash_getopt(slash, "p:b:t:a:rh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'b':
			backend = slash->optarg;
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	if (remain != 1)
		return SLASH_EUSAGE;

	remotepath = slash->argv[index];

	/* Connect to server */
	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	ret = btp_client_remove(btp, backend, remotepath);
	if (ret != BTP_EOK)
		UPLOG_ERR("Could not remove %s: %s\n", remotepath, btp_error(ret));

	/* Close connection */
	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}

int cmd_btp_move(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c;
	char *endptr;

	struct btp_context *btp;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t options = CSP_O_NONE;
	unsigned int attempts = DEFAULT_ATTEMPTS;

	char *backend = (char *)DEFAULT_BACKEND;
	char *remotepath = NULL;
	char *localpath = NULL;

	/* Parse command line arguments */
	while ((c = slash_getopt(slash, "p:b:t:a:rh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'b':
			backend = slash->optarg;
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	if (remain != 2)
		return SLASH_EUSAGE;

	localpath = slash->argv[index];
	remotepath = slash->argv[index + 1];

	/* Connect to server */
	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	ret = btp_client_move(btp, backend, localpath, remotepath);
	if (ret != BTP_EOK)
		UPLOG_ERR("Could not move %s to %s: %s\n", localpath, remotepath, btp_error(ret));

	/* Close connection */
	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}

int cmd_btp_copy(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c;
	char *endptr;

	struct btp_context *btp;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t options = CSP_O_NONE;
	unsigned int attempts = DEFAULT_ATTEMPTS;

	char *backend = (char *)DEFAULT_BACKEND;
	char *remotepath = NULL;
	char *localpath = NULL;

	/* Parse command line arguments */
	while ((c = slash_getopt(slash, "p:b:t:a:rh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'b':
			backend = slash->optarg;
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	if (remain != 2)
		return SLASH_EUSAGE;

	localpath = slash->argv[index];
	remotepath = slash->argv[index + 1];

	/* Connect to server */
	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	ret = btp_client_copy(btp, backend, localpath, remotepath);
	if (ret != BTP_EOK)
		UPLOG_ERR("Could not copy %s to %s: %s\n", localpath, remotepath, btp_error(ret));

	/* Close connection */
	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}

int cmd_btp_shell(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c, i;
	char *endptr;

	struct btp_context *btp;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t options = CSP_O_NONE;
	uint32_t count = DEFAULT_COUNT;
	bool singlechar = false;
	unsigned int attempts = DEFAULT_ATTEMPTS;

	char shell[SHELL_INP_SIZE] = "";
	int shellp = 0;

	while ((c = slash_getopt(slash, "p:c:t:a:rjh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'c':
			count = atoi(slash->optarg);
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'j':
			singlechar = true;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	for (i = 0; i < remain; i++) {
		if (i > 0)
			shellp = shell - strncat(shell, " ", SHELL_INP_SIZE - shellp);
		shellp = shell - strncat(shell, slash->argv[index + i], SHELL_INP_SIZE - shellp);
	}

	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	ret = btp_client_shell(btp, count, shell, singlechar);
	if (ret != BTP_EOK)
		UPLOG_ERR("Could not execute %s: %s\n", shell, btp_error(ret));

	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}

int cmd_btp_mkdir(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c;
	char *endptr;

	struct btp_context *btp;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t options = CSP_O_NONE;
	unsigned int attempts = DEFAULT_ATTEMPTS;

	char *backend = (char *)DEFAULT_BACKEND;
	char *remotepath = NULL;

	/* Parse command line arguments */
	while ((c = slash_getopt(slash, "p:b:t:a:rh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'b':
			backend = slash->optarg;
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	if (remain != 1)
		return SLASH_EUSAGE;

	remotepath = slash->argv[index];

	/* Connect to server */
	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	ret = btp_client_mkdir(btp, backend, remotepath);
	if (ret != BTP_EOK)
		UPLOG_ERR("Could not mkdir %s: %s\n", remotepath, btp_error(ret));

	/* Close connection */
	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}

int cmd_btp_rmdir(struct slash *slash)
{
	struct sl_btp_client_command_context *ctx = slash->context;

	int ret, remain, index, c;
	char *endptr;

	struct btp_context *btp;
	uint8_t node;
	uint8_t port = ctx ? ctx->port : BTP_DEFAULT_PORT;
	uint32_t timeout = DEFAULT_TIMEOUT;
	uint32_t options = CSP_O_NONE;
	unsigned int attempts = DEFAULT_ATTEMPTS;

	char *backend = (char *)DEFAULT_BACKEND;
	char *remotepath = NULL;

	/* Parse command line arguments */
	while ((c = slash_getopt(slash, "p:b:t:a:rh")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(slash->optarg);
			break;
		case 'b':
			backend = slash->optarg;
			break;
		case 't':
			timeout = atoi(slash->optarg);
			break;
		case 'a':
			attempts = atoi(slash->optarg);
			break;
		case 'r':
			options = CSP_O_RDP;
			break;
		case 'h':
			return SLASH_EHELP;
		case '?':
			if (slash->optopt == 'c')
				UPLOG_ERR("Option -%c requires an argument.\n", slash->optopt);
			/* FALLTHROUGH */
		default:
			return SLASH_EUSAGE;
		}
	}

	remain = slash->argc - slash->optind;
	index = slash->optind;

	if (ctx) {
		node = *ctx->node;
	} else {
		if (remain < 1)
			return SLASH_EUSAGE;

		node = (uint8_t)strtoul(slash->argv[index], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		remain--;
		index++;
	}

	if (node > CSP_ID_HOST_MAX)
		return SLASH_EUSAGE;

	if (remain != 1)
		return SLASH_EUSAGE;

	remotepath = slash->argv[index];

	/* Connect to server */
	btp = btp_client_connect(node, port, timeout, options, attempts);
	if (!btp) {
		UPLOG_ERR("Failed to connect to BTP server\n");
		return SLASH_EIO;
	}

	ret = btp_client_rmdir(btp, backend, remotepath);
	if (ret != BTP_EOK)
		UPLOG_ERR("Could not rmdir %s: %s\n", remotepath, btp_error(ret));

	/* Close connection */
	btp_client_disconnect(btp);

	return SLASH_SUCCESS;
}


/* Generate generic 'btp ...' commands */
SL_BTP_CLIENT_COMMANDS_GENERATE_TOPLEVEL();
