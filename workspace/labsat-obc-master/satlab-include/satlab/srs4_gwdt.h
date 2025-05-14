/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file srs4_gwdt.h
 *
 * This file provides functions for resetting and reading the ground watchdog
 * timer.
 */

#ifndef _SL_SRS4_CLIENT_GWDT_H_
#define _SL_SRS4_CLIENT_GWDT_H_

#include <stdint.h>

#include <prop-client/prop_client.h>
#include <prop-client/prop_proto.h>
#include <satlab/srs4_props.h>

/* Random key to reset GWDT */
#define SL_SRS4_GWDT_RESET_KEY	0xCCB1180C

/**
 * @brief Reset ground watchdog timer
 *
 * Helper function to reset the ground watchdog timer property.
 *
 * @param node CSP address of the SRS-4 node.
 * @param timeout Timeout of the command in milliseconds.
 * @param key Must be SL_SRS4_GWDT_RESET_KEY.
 *
 * @returns 0 on success, and a negative error code on error.
 */
static inline int sl_srs4_gwdt_reset(uint8_t node, uint32_t timeout, uint32_t key)
{
	return sl_prop_remote_set_uint32(node, SL_PROP_DEFAULT_PORT, timeout, SL_SRS4_PROP_SYS_GWDT_RESET, key);
}

/**
 * @brief Read ground watchdog timer
 *
 * Helper function to read the current ground watchdog timer counter. The
 * counter holds the remaing time in seconds before the ground watchdog timer
 * resets the system.
 *
 * @param node CSP address of the SRS-4 node.
 * @param timeout Timeout of the command in milliseconds.
 * @param counter Pointer where the current counter is stored.
 *
 * @returns 0 on success, and a negative error code on error.
 */
static inline int sl_srs4_gwdt_counter(uint8_t node, uint32_t timeout, uint32_t *counter)
{
	return sl_prop_remote_get_uint32(node, SL_PROP_DEFAULT_PORT, timeout, SL_SRS4_PROP_SYS_GWDT_COUNTER, counter);
}

#endif /* _SL_SRS4_CLIENT_GWDT_H_ */
