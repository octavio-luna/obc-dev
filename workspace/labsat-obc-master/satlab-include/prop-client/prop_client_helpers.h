/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file
 *
 * This file contains basic functions to get/set a single remote property. They
 * are all wrappers around sl_prop_remote_get/set, but should be used where
 * possible because they perform compile time validation of the function
 * arguments.
 */

#ifndef _SL_PROP_CLIENT_HELPERS_H_
#define _SL_PROP_CLIENT_HELPERS_H_

#include <stdint.h>
#include <errno.h>

#include <prop-client/prop_spec.h>

/**
 * @brief Set remote boolean property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_bool(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, bool val);

/**
 * @brief Get remote boolean property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_bool(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, bool *val);

/**
 * @brief Set remote signed 8-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_int8(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int8_t val);

/**
 * @brief Get remote signed 8-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_int8(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int8_t *val);

/**
 * @brief Set remote signed 16-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_int16(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int16_t val);

/**
 * @brief Get remote signed 16-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_int16(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int16_t *val);

/**
 * @brief Set remote signed 32-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_int32(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int32_t val);

/**
 * @brief Get remote signed 32-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_int32(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int32_t *val);

/**
 * @brief Set remote signed 64-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_int64(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int64_t val);

/**
 * @brief Get remote signed 64-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_int64(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int64_t *val);

/**
 * @brief Set remote unsigned 8-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_uint8(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint8_t val);

/**
 * @brief Get remote unsigned 8-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_uint8(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint8_t *val);

/**
 * @brief Set remote unsigned 16-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_uint16(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint16_t val);

/**
 * @brief Get remote unsigned 16-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_uint16(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint16_t *val);

/**
 * @brief Set remote unsigned 32-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_uint32(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint32_t val);

/**
 * @brief Get remote unsigned 32-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_uint32(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint32_t *val);

/**
 * @brief Set remote unsigned 64-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_uint64(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint64_t val);

/**
 * @brief Get remote unsigned 64-bit property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_uint64(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint64_t *val);

/**
 * @brief Set remote single precision floating point property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_float(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, float val);

/**
 * @brief Get remote single preceision floating point property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_float(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, float *val);

/**
 * @brief Set remote double precision floating point property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_double(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, double val);

/**
 * @brief Get remote double preceision floating point property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_double(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, double *val);

/**
 * @brief Set remote string property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 * @param size		Size of value string. Must match property size.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_string(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, const char *val, size_t size);

/**
 * @brief Get remote string property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 * @param size		Size of value string. Must match property size.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_string(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, char *val, size_t size);

/**
 * @brief Set remote binary property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to set.
 * @param val		New value of remote property.
 * @param size		Size of binary value. Must match property size.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_set_binary(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, const uint8_t *val, size_t size);

/**
 * @brief Get remote binary property
 *
 * @param node       	CSP address of the property node.
 * @param port       	CSP port of the property node.
 * @param timeout    	Timeout of the command in milliseconds.
 * @param id		ID of remote property to get.
 * @param val		Pointer to reply value.
 * @param size		Size of binary value. Must match property size.
 *
 * @returns 0 on success, and a negative error code on error.
 */
int sl_prop_remote_get_binary(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint8_t *val, size_t size);

#endif /* _SL_PROP_CLIENT_HELPERS_H_ */
