/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file srs4_shell.h
 *
 * This file provides functions for running debug shell commands remotely. It
 * is meant as a debugging aid and should not be required during normal
 * operation of the device.
 */

#ifndef _SL_SRS4_CLIENT_SHELL_H_
#define _SL_SRS4_CLIENT_SHELL_H_

#include <stdint.h>

/**
 * @brief Run debug shell command
 *
 * This function allows debug shell commands to be run on the Polaris system.
 * The command is evaluated on the remote systems, and the command return value
 * is returned. Output from the command is not available.
 *
 * @param node CSP address of the property node.
 * @param timeout Timeout of the command in milliseconds.
 * @param cmd Command to execute
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_srs4_shell_run(uint8_t node, uint32_t timeout, const char *cmd);

#endif /* _SL_SRS4_CLIENT_SHELL_H_ */
