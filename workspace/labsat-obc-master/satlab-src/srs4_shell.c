/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <satlab/srs4_shell.h>

#include <errno.h>

#include <csp/csp.h>
#include <endian.h>

#include <satlab/srs4.h>

int sl_srs4_shell_run(uint8_t node, uint32_t timeout, const char *cmd)
{
	int ret;
	sl_srs4_shell_run_req_t request;
	sl_srs4_shell_run_rep_t reply;

	request.type = SL_SRS4_SHELL_RUN_REQ;
	request.flags = 0;
	request.key = csp_hton32(SL_SRS4_SHELL_RUN_KEY);

	if (strlen(cmd) >= sizeof(request.cmd))
		return -ENOSPC;

	strncpy((char *)request.cmd, cmd, sizeof(request.cmd) - 1);
	request.cmd[sizeof(request.cmd) - 1] = '\0';

	ret = csp_transaction(CSP_PRIO_NORM, node, SL_SRS4_PORT_SHELL, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_SRS4_SHELL_RUN_REP)
		return -EINVAL;

	if (reply.error != SL_SRS4_ERR_NONE)
		return -reply.error;

	return 0;
}

