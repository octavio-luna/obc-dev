/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file
 *
 * This file contains slash (https://github.com/satlab/slash) debug commands
 * for the remote property functions. The SL_PROP_CLIENT_COMMANDS_GENERATE macro is used by subsystem command implementations to generate subcommands with the correct
 */

#ifndef _SL_PROP_CLIENT_COMMANDS_H_
#define _SL_PROP_CLIENT_COMMANDS_H_

#include <stdint.h>

#include <slash/slash.h>

#include <prop-client/prop_proto.h>

/** Slash property command context */
struct sl_prop_client_command_context {
	uint8_t *node;
	uint8_t port;
	uint32_t *timeout;
	const struct prop_subsys_spec *spec;
};

/**
 * @brief Generate slash command definitions for subsystem
 *
 * This adds property commands to a parent slash group. The commands are called
 * "<parent> prop ...".
 *
 * @param _cmdgroup Parent command group name.
 * @param _node Pointer to CSP node address.
 * @param _port CSP node port.
 * @param _timeout Pointer to CSP timeout value.
 * @param _spec Pointer to property subsystem spec.
 */
#define SL_PROP_CLIENT_COMMANDS_GENERATE(_cmdgroup, _node, _port, _timeout, _spec) \
	static struct sl_prop_client_command_context sl_cmdpctx_ ## _cmdgroup = { \
		.node = _node, \
		.port = _port, \
		.timeout = _timeout, \
		.spec = _spec, \
	}; \
	slash_command_subgroup(_cmdgroup, prop, "Properties commands"); \
	slash_command_subsub_ex(_cmdgroup, prop, get, cmd_prop_client_get, "<group.property> [group.property]", "Get remote properties", 0, &sl_cmdpctx_ ## _cmdgroup); \
	slash_command_subsub_ex(_cmdgroup, prop, set, cmd_prop_client_set, "<group.property> <value>", "Set remote property", 0, &sl_cmdpctx_ ## _cmdgroup); \
	slash_command_subsub_ex(_cmdgroup, prop, reset, cmd_prop_client_reset, "<group>", "Reset properties to default values", 0, &sl_cmdpctx_ ## _cmdgroup); \
	slash_command_subsub_ex(_cmdgroup, prop, list, cmd_prop_client_list, "[group ...]", "List property groups or group contents", 0, &sl_cmdpctx_ ## _cmdgroup); \
	slash_command_subsub_ex(_cmdgroup, prop, save, cmd_prop_client_save, "[-f] <group>", "Save property group", 0, &sl_cmdpctx_ ## _cmdgroup); \
	slash_command_subsub_ex(_cmdgroup, prop, load, cmd_prop_client_load, "[-f] <group>", "Load property group", 0, &sl_cmdpctx_ ## _cmdgroup); \
	slash_command_subsub_ex(_cmdgroup, prop, erase, cmd_prop_client_erase, "[-f] <group>", "Erase property group", 0, &sl_cmdpctx_ ## _cmdgroup); \
	slash_command_subsub_ex(_cmdgroup, prop, lock, cmd_prop_client_lock, "[-f] <group>", "Lock property group", 0, &sl_cmdpctx_ ## _cmdgroup); \
	slash_command_subsub_ex(_cmdgroup, prop, unlock, cmd_prop_client_unlock, "[-f] <group>", "Unlock property group", 0, &sl_cmdpctx_ ## _cmdgroup);

int cmd_prop_client_get(struct slash *slash);

int cmd_prop_client_set(struct slash *slash);

int cmd_prop_client_reset(struct slash *slash);

int cmd_prop_client_list(struct slash *slash);

int cmd_prop_client_save(struct slash *slash);

int cmd_prop_client_load(struct slash *slash);

int cmd_prop_client_erase(struct slash *slash);

int cmd_prop_client_lock(struct slash *slash);

int cmd_prop_client_unlock(struct slash *slash);

#endif /* _SL_PROP_CLIENT_COMMANDS_H_ */
