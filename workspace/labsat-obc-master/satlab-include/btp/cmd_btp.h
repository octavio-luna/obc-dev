/*
 * Copyright (c) 2009-2022 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_BTP_CLIENT_CMD_H_
#define _SL_BTP_CLIENT_CMD_H_

#include <slash/slash.h>

int cmd_btp_push(struct slash *slash);

int cmd_btp_pull(struct slash *slash);

int cmd_btp_list(struct slash *slash);

int cmd_btp_remove(struct slash *slash);

int cmd_btp_move(struct slash *slash);

int cmd_btp_copy(struct slash *slash);

int cmd_btp_shell(struct slash *slash);

int cmd_btp_mkdir(struct slash *slash);

int cmd_btp_rmdir(struct slash *slash);

struct sl_btp_client_command_context {
	uint8_t *node;
	uint8_t port;
};

#define SL_BTP_CLIENT_COMMANDS_GENERATE_GENERIC(_ctx, _nodearg, _slash_macro, ...) \
	_slash_macro(__VA_ARGS__, push, cmd_btp_push, "[options] " _nodearg "<local-path> <remote-path>", \
			     "Push file to target\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -b BACKEND,\tUse BACKEND as backend\n" \
			     " -s BLOCKSIZE,\tTransfer data in BLOCKSIZE sized blocks\n" \
			     " -c COUNT,\tRequest COUNT blocks at a time\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -i TIMEOUT,\tSet initial checksum timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -q,\t\tQuiet mode. Do not print progress bar\n" \
			     " -r,\t\tEnable RDP for transfer\n" \
			     " -f,\t\tDelete data and map file before starting transfer\n", \
			     0, _ctx); \
	_slash_macro(__VA_ARGS__, pull, cmd_btp_pull, "[options] " _nodearg "<remote-path> [local-path]", \
			     "Pull file from target\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -b BACKEND,\tUse BACKEND as backend\n" \
			     " -s BLOCKSIZE,\tTransfer data in BLOCKSIZE sized blocks\n" \
			     " -c COUNT,\tRequest COUNT blocks at a time\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -i TIMEOUT,\tSet initial checksum timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -q,\t\tQuiet mode. Do not print progress bar\n" \
			     " -r,\t\tEnable RDP for transfer\n" \
			     " -f,\t\tDelete data and map file before starting transfer\n", \
			     0, _ctx); \
	_slash_macro(__VA_ARGS__, list, cmd_btp_list, "[options] " _nodearg "<remote-path>", \
			     "List remote path\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -b BACKEND,\tUse BACKEND as backend\n" \
			     " -c COUNT,\tRequest COUNT blocks at a time\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -r,\t\tEnable RDP for transfer\n", \
			     0, _ctx); \
	_slash_macro(__VA_ARGS__, remove, cmd_btp_remove, "[options] " _nodearg "<remote-path>", \
			     "Remove remote file\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -b BACKEND,\tUse BACKEND as backend\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -r,\t\tEnable RDP for transfer\n", \
			     0, _ctx); \
	_slash_macro(__VA_ARGS__, copy, cmd_btp_copy, "[options] " _nodearg "<remote-source> <remote-dest>", \
			     "Copy remote file\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -b BACKEND,\tUse BACKEND as backend\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -r,\t\tEnable RDP for transfer\n", \
			     0, _ctx); \
	_slash_macro(__VA_ARGS__, move, cmd_btp_move, "[options] " _nodearg "<remote-source> <remote-dest>", \
			     "Move remote file\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -b BACKEND,\tUse BACKEND as backend\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -r,\t\tEnable RDP for transfer\n", \
			     0, _ctx); \
	_slash_macro(__VA_ARGS__, shell, cmd_btp_shell, "[options] " _nodearg "[command]", \
			     "Run shell command on target\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -c COUNT,\tRequest COUNT blocks at a time\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -r,\t\tEnable RDP for transfer\n" \
			     " -j,\t\tSingle character terminal mode\n", \
			     0, _ctx); \
	_slash_macro(__VA_ARGS__, mkdir, cmd_btp_mkdir, "[options] " _nodearg "<remote-path>", \
			     "Make directory on target\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -b BACKEND,\tUse BACKEND as backend\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -r,\t\tEnable RDP for transfer\n", \
			     0, _ctx); \
	_slash_macro(__VA_ARGS__, rmdir, cmd_btp_rmdir, "[options] " _nodearg "<remote-path>", \
			     "Remove directory on target\n\n" \
			     "Options:\n" \
			     " -h,\t\tPrints this help message\n" \
			     " -p PORT,\tConnect to PORT\n" \
			     " -b BACKEND,\tUse BACKEND as backend\n" \
			     " -t TIMEOUT,\tSet timeout to TIMEOUT ms\n" \
			     " -a ATTEMPTS,\tRetry connection up to ATTEMPTS tries\n" \
			     " -r,\t\tEnable RDP for transfer\n", \
			     0, _ctx);

#define SL_BTP_CLIENT_COMMANDS_GENERATE(_cmdgroup, _node, _port) \
	static struct sl_btp_client_command_context sl_btp_cmdcctx_ ## _cmdgroup = { \
		.node = _node, \
		.port = _port, \
	}; \
	slash_command_subgroup(_cmdgroup, btp, "Block Transfer Protocol commands"); \
	SL_BTP_CLIENT_COMMANDS_GENERATE_GENERIC(&sl_btp_cmdcctx_ ## _cmdgroup, "", slash_command_subsub_ex, _cmdgroup, btp)

#define SL_BTP_CLIENT_COMMANDS_GENERATE_TOPLEVEL() \
	slash_command_group(btp, "Block Transfer Protocol commands"); \
	SL_BTP_CLIENT_COMMANDS_GENERATE_GENERIC(NULL, "<node> ", slash_command_sub_ex, btp)

#endif /* _BTP_CLIENT_CMD_H_ */
