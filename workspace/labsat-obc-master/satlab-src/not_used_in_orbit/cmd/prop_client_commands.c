/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <prop-client/prop_client_commands.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <slash/slash.h>

#include <prop-client/prop_client.h>

static int prop_client_group_print_iter(const struct prop_group_spec *group, void *arg)
{
	UPLOG_INFO("%-16s%-s\n", group->name, group->desc);

	return 0;
}

static int prop_client_print_iter(const struct prop_spec *prop, void *arg)
{
	char valstr[128] = "?";
	const char *type;
	const char *unit;
	struct sl_prop_query *query = (struct sl_prop_query *)arg;


	type = sl_prop_spec_type_string(prop);
	unit = prop->type.unit ? prop->type.unit : "";
	if (prop->flags & PROP_FLAG_SECRET) {
		strncpy(valstr, "<hidden>", sizeof(valstr) - 1);
	} else {
		sl_prop_remote_query_get_reply_to_string(query, prop, valstr, sizeof(valstr));
	}

	UPLOG_INFO("%-16s%-8s%s %s\n", prop->name, type, valstr, unit);

	return 0;
}

int cmd_prop_client_get(struct slash *slash)
{
	int i, ret;
	const struct prop_spec *spec;
	char valstr[128];
	struct sl_prop_client_command_context *ctx = slash->context;

	if (slash->argc < 2)
		return SLASH_EUSAGE;

	for (i = 1; i < slash->argc; i++) {
		spec = sl_prop_spec_find_by_name(ctx->spec, slash->argv[i]);
		if (!spec) {
			UPLOG_ERR("No such property \"%s\"\n", slash->argv[i]);
			return SLASH_ENOENT;
		}

		ret = sl_prop_remote_get_to_string(*ctx->node, ctx->port, *ctx->timeout, spec, valstr, sizeof(valstr));
		if (ret < 0) {
			UPLOG_ERR("Failed to read property value: %s\n", strerror(-ret));
			return SLASH_EINVAL;
		}

		UPLOG_INFO("%s\n", valstr);
	}

	return 0;
}

int cmd_prop_client_set(struct slash *slash)
{
	int ret;
	const struct prop_spec *spec;
	struct sl_prop_client_command_context *ctx = slash->context;

	if (slash->argc != 3)
		return SLASH_EUSAGE;

	spec = sl_prop_spec_find_by_name(ctx->spec, slash->argv[1]);
	if (!spec) {
		UPLOG_ERR("No such property \"%s\"\n", slash->argv[1]);
		return SLASH_ENOENT;
	}

	ret = sl_prop_remote_set_from_string(*ctx->node, ctx->port, *ctx->timeout, spec, slash->argv[2], strlen(slash->argv[2]) + 1);
	if (ret < 0) {
		if (ret == -EINVAL) {
			UPLOG_ERR("Invalid value string\n");
		} else if (ret == -ENOENT) {
			UPLOG_ERR("Invalid property index\n");
		} else if (ret == -ERANGE) {
			UPLOG_ERR("Value is outside valid property range\n");
		} else if (ret == -ETIMEDOUT) {
			UPLOG_ERR("Timeout while waiting for reply\n");
		} else {
			UPLOG_ERR("Failed to set property value: %s\n", strerror(-ret));
		}
		return SLASH_EINVAL;
	}

	return 0;
}

int cmd_prop_client_reset(struct slash *slash)
{
	int ret;
	const struct prop_spec *spec;
	const struct prop_group_spec *group_spec;
	prop_id_t id;
	struct sl_prop_client_command_context *ctx = slash->context;

	if (slash->argc < 2)
		return SLASH_EUSAGE;

	if (strchr(slash->argv[1], '.')) {
		spec = sl_prop_spec_find_by_name(ctx->spec, slash->argv[1]);
		if (!spec) {
			UPLOG_ERR("No such property \"%s\"\n", slash->argv[1]);
			return SLASH_ENOENT;
		}

		id = spec->id;
	} else {
		group_spec = sl_prop_group_spec_find_by_name(ctx->spec, slash->argv[1]);
		if (!group_spec) {
			UPLOG_ERR("No such property group \"%s\"\n", slash->argv[1]);
			return SLASH_ENOENT;
		}

		id = group_spec->id;
	}

	ret = sl_prop_remote_reset(*ctx->node, ctx->port, *ctx->timeout, id);
	if (ret < 0) {
		if (ret == -EINVAL) {
			UPLOG_ERR("Invalid value string\n");
		} else if (ret == -ENOENT) {
			UPLOG_ERR("Invalid property index\n");
		} else if (ret == -ERANGE) {
			UPLOG_ERR("Value is outside valid property range\n");
		} else if (ret == -ETIMEDOUT) {
			UPLOG_ERR("Timeout while waiting for reply\n");
		} else {
			UPLOG_ERR("Failed to set property value: %s\n", strerror(-ret));
		}
		return SLASH_EINVAL;
	}

	return 0;
}

int cmd_prop_client_list(struct slash *slash)
{
	int ret;
	size_t i;
	const struct prop_group_spec *group_spec;
	struct sl_prop_query query;
	struct sl_prop_client_command_context *ctx = slash->context;

	uint8_t txbuf[16];
	uint8_t rxbuf[512];

	bool header_shown = false;

	if (slash->argc < 2) {
		UPLOG_INFO("\033[1m%-16s%-8s\033[0m\n", "Group", "Description");
		sl_prop_group_spec_iterate(ctx->spec, prop_client_group_print_iter, NULL);
	} else {
		for (i = 1; i < slash->argc; i++) {
			group_spec = sl_prop_group_spec_find_by_name(ctx->spec, slash->argv[i]);
			if (!group_spec) {
				UPLOG_ERR("No such property group \"%s\"\n", slash->argv[i]);
				return SLASH_ENOENT;
			}

			sl_prop_remote_query_create_static(&query, txbuf, sizeof(txbuf), rxbuf, sizeof(rxbuf));
			sl_prop_remote_query_get(&query, group_spec->id);
			ret = sl_prop_remote_query_send(&query, *ctx->node, ctx->port, *ctx->timeout);
			if (ret < 0) {
				fUPLOG_ERR(stderr, "Query failed: %s\n", strerror(-ret));
				return SLASH_EIO;
			}

			if (!header_shown) {
				UPLOG_INFO("\033[1m%-16s%-8s%s\033[0m\n", "Property", "Type", "Value");
				header_shown = true;
			}

			sl_prop_spec_iterate(group_spec, prop_client_print_iter, &query);
		}
	}

	return 0;
}

int cmd_prop_client_save(struct slash *slash)
{
	int c, ret;
	const struct prop_group_spec *group;
	bool fallback = false;
	struct sl_prop_client_command_context *ctx = slash->context;

	while ((c = slash_getopt(slash, "f")) != -1) {
		switch (c) {
		case 'f':
			fallback = true;
			break;
		default:
			return SLASH_EUSAGE;
		}
	}

	if (slash->argc - slash->optind != 1)
		return SLASH_EUSAGE;

	group = sl_prop_group_spec_find_by_name(ctx->spec, slash->argv[slash->optind]);
	if (!group) {
		UPLOG_ERR("No such property group \"%s\"\n", slash->argv[slash->optind]);
		return SLASH_ENOENT;
	}

	ret = sl_prop_remote_save(*ctx->node, ctx->port, *ctx->timeout, group->id, fallback);
	if (ret < 0) {
		UPLOG_ERR("Failed to save group: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	return 0;
}

int cmd_prop_client_load(struct slash *slash)
{
	int c, ret;
	const struct prop_group_spec *group;
	bool fallback = false;
	struct sl_prop_client_command_context *ctx = slash->context;

	while ((c = slash_getopt(slash, "f")) != -1) {
		switch (c) {
		case 'f':
			fallback = true;
			break;
		default:
			return SLASH_EUSAGE;
		}
	}

	if (slash->argc - slash->optind != 1)
		return SLASH_EUSAGE;

	group = sl_prop_group_spec_find_by_name(ctx->spec, slash->argv[slash->optind]);
	if (!group) {
		UPLOG_ERR("No such property group \"%s\"\n", slash->argv[slash->optind]);
		return SLASH_ENOENT;
	}

	ret = sl_prop_remote_load(*ctx->node, ctx->port, *ctx->timeout, group->id, fallback);
	if (ret < 0) {
		UPLOG_ERR("Failed to load group: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	return 0;
}

int cmd_prop_client_erase(struct slash *slash)
{
	int c, ret;
	const struct prop_group_spec *group;
	bool fallback = false;
	struct sl_prop_client_command_context *ctx = slash->context;

	while ((c = slash_getopt(slash, "f")) != -1) {
		switch (c) {
		case 'f':
			fallback = true;
			break;
		default:
			return SLASH_EUSAGE;
		}
	}

	if (slash->argc - slash->optind != 1)
		return SLASH_EUSAGE;

	group = sl_prop_group_spec_find_by_name(ctx->spec, slash->argv[slash->optind]);
	if (!group) {
		UPLOG_ERR("No such property group \"%s\"\n", slash->argv[slash->optind]);
		return SLASH_ENOENT;
	}

	ret = sl_prop_remote_erase(*ctx->node, ctx->port, *ctx->timeout, group->id, fallback);
	if (ret < 0) {
		UPLOG_ERR("Failed to erase group: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	return 0;
}

int cmd_prop_client_lock(struct slash *slash)
{
	int c, ret;
	const struct prop_group_spec *group;
	bool fallback = false;
	struct sl_prop_client_command_context *ctx = slash->context;

	while ((c = slash_getopt(slash, "f")) != -1) {
		switch (c) {
		case 'f':
			fallback = true;
			break;
		default:
			return SLASH_EUSAGE;
		}
	}

	if (slash->argc - slash->optind != 1)
		return SLASH_EUSAGE;

	group = sl_prop_group_spec_find_by_name(ctx->spec, slash->argv[slash->optind]);
	if (!group) {
		UPLOG_ERR("No such property group \"%s\"\n", slash->argv[slash->optind]);
		return SLASH_ENOENT;
	}

	ret = sl_prop_remote_lock(*ctx->node, ctx->port, *ctx->timeout, group->id, fallback);
	if (ret < 0) {
		UPLOG_ERR("Failed to lock group: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	return 0;
}

int cmd_prop_client_unlock(struct slash *slash)
{
	int c, ret;
	const struct prop_group_spec *group;
	bool fallback = false;
	struct sl_prop_client_command_context *ctx = slash->context;

	while ((c = slash_getopt(slash, "f")) != -1) {
		switch (c) {
		case 'f':
			fallback = true;
			break;
		default:
			return SLASH_EUSAGE;
		}
	}

	if (slash->argc - slash->optind != 1)
		return SLASH_EUSAGE;

	group = sl_prop_group_spec_find_by_name(ctx->spec, slash->argv[slash->optind]);
	if (!group) {
		UPLOG_ERR("No such property group \"%s\"\n", slash->argv[slash->optind]);
		return SLASH_ENOENT;
	}

	ret = sl_prop_remote_unlock(*ctx->node, ctx->port, *ctx->timeout, group->id, fallback);
	if (ret < 0) {
		UPLOG_ERR("Failed to unlock group: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	return 0;
}
