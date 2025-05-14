/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file
 *
 * This file defines the main functions for accessing properties on remote systems.
 *
 * Properties are identified using a 16-bit ID, with 5 bits for group and 7
 * bits for property in that group. The remaining 4 bits encode the type.
 * Groups are identified with a similar ID, but with the type and property bits
 * set to zero:
 *
 * @code
 *  0                   1
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Type  |  Group  |   Property  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * @endcode
 *
 * Each system client library uses a C source and header file which defines
 * available properties on the system, and specifies the type and name of each
 * property. The header file is generated from the properties JSON file found
 * in the `props` directory of the client tarball, using the `propgen.py`
 * script.  This happens automatically when using the supplied `wscript`. If
 * building manually, the following example shows the command to generate
 * properties for `polaris-client`, but the command is the same for other
 * subsystems, except for changed file names:
 *
 * @code
 * ./prop-client/tools/propgen.py -i $PWD/polaris_props.h -s $PWD/polaris_props.c polaris-client/props/polaris_props.json
 * @endcode
 *
 * Note that the -i and -s arguments expect absolute paths.
 */

#ifndef _SL_PROP_CLIENT_H_
#define _SL_PROP_CLIENT_H_

#include <stdint.h>
#include <errno.h>

#include <prop-client/prop_spec.h>
#include <prop-client/prop_query.h>
#include <prop-client/prop_client_helpers.h>

/**
 * @brief Parse string to boolean value
 *
 * "1", "yes", and "true" are parsed to value=1 and "0", "no", and "false" are
 * parsed to value=0. Any other value return -EINVAL and value is not changed.
 *
 * @param string	Pointer to boolean string to parse.
 * @param size		Size of the value pointed to by string.
 * @param value		Parsed boolean value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_parse_bool(const char *string, size_t size, uint8_t *value);

/**
 * @brief Get remote property value to string
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param spec		Pointer to the property spec of the property to get.
 * @param string	Pointer to string buffer where the result is stored.
 * @param size		Size of the value pointed to by string.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_to_string(uint8_t node, uint8_t port, uint32_t timeout, const struct prop_spec *spec, char *string, size_t size);

/**
 * @brief Set remote property value from string
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param spec		Pointer to the property spec of the property to set.
 * @param string	Pointer to the string representation of the new
 *			property value. Must match the type of the property.
 * @param size		Size of the value pointed to by string.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_from_string(uint8_t node, uint8_t port, uint32_t timeout, const struct prop_spec *spec, const char *string, size_t size);

/**
 * @brief Get remote property value
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of the property to get.
 * @param value		Pointer to the property value. Must match the type of
 * 			the property.
 * @param size		Size of the value pointed to by value. Must match the
 * 			property type size.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, void *value, size_t size);

/**
 * @brief Set remote property value
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of the property to set.
 * @param value		Pointer to the new property value. Must match the type
 * 			of the property.
 * @param size		Size of the value pointed to by value. Must match the
 * 			property type size.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, const void *value, size_t size);

/**
 * @brief Reset remote property or group to default value
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		Property ID. Can be a single property or a group ID.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_reset(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id);

/**
 * @brief Save property group on remote system
 *
 * If the group supports locking, it must be unlocked before calling this
 * function.
 *
 * @warning Be very careful with this command. Modifying and saving certain
 * properties, such as the CSP address, could render the system unresponsible.
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param group      	Property ID of the group to save
 * @param fallback   	If true, the group is saved to the fallback store.
 *              	Otherwise, it is saved to the boot store.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_save(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback);

/**
 * @brief Load property group on remote system
 *
 * @param node		CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout	Timeout of the command in milliseconds.
 * @param group		Property ID of the group to load
 * @param fallback	If true, the group is loaded from the fallback store.
 *			Otherwise, it is loaded from the boot store.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_load(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback);

/**
 * @brief Erase saved property group on remote system
 *
 * @warning Be very careful with this command. Modifying and saving certain
 * properties, such as the CSP address, could render the system unresponsible.
 *
 * @param node		CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout	Timeout of the command in milliseconds.
 * @param group		Property ID of the group to load
 * @param fallback	If true, the fallback store is erased. Otherwise, the
 *			boot store is erased.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_erase(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback);

/**
 * @brief Lock property group on remote system
 *
 * @param node		CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout	Timeout of the command in milliseconds.
 * @param group		Property ID of the group to load
 * @param fallback	If true, the fallback store is locked. Otherwise, the
 *			boot store is lock.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_lock(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback);

/**
 * @brief Unlock property group on remote system
 *
 * @warning Be very careful with this command. Modifying and saving certain
 * properties, such as the CSP address, could render the system unresponsible.
 *
 * @param node		CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout	Timeout of the command in milliseconds.
 * @param group		Property ID of the group to load
 * @param fallback	If true, the fallback store is unlocked. Otherwise, the
 *			boot store is unlock.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_unlock(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback);

#endif /* _SL_PROP_CLIENT_H_ */
