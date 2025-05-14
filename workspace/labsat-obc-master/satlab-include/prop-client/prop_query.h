/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file
 *
 * The property query interface allows clients to get or set multiple
 * properties in a single transaction. A query is either a GET or a SET query,
 * determined by the first call to either sl_prop_remote_query_get/set.
 *
 * The code excerpt below shows an example of reading out the Polaris FPGA
 * temperature and serial number in one query. Error handling has been omitted
 * for clarity.
 *
 * @code{.cpp}
 * int16_t temp;
 * uint32_t serial;
 * struct sl_prop_query query;
 * uint8_t txbuf[128], rxbuf[128];
 *
 * // Prepare query and buffers
 * sl_prop_remote_query_create_static(&query, txbuf, sizeof(txbuf), rxbuf, sizeof(rxbuf));
 * sl_prop_remote_query_get(&query, SL_PL_PROP_TM_TEMP_FPGA);
 * sl_prop_remote_query_get(&query, SL_PL_PROP_SYS_SERIAL);
 *
 * // Send query message
 * sl_prop_remote_query_send(&query, node, timeout);
 *
 * // Read reply and free query object
 * sl_prop_remote_query_get_reply(&query, SL_PL_PROP_TM_TEMP_FPGA, &temp, sizeof(temp));
 * sl_prop_remote_query_get_reply(&query, SL_PL_PROP_SYS_SERIAL, &serial, sizeof(serial));
 * sl_prop_remote_query_destroy(&query);
 *
 * // Show results
 * printf("Serial number: %08x\n", serial);
 * printf("FPGA temperature: %hd\n", temp);
 * @endcode
 */

#ifndef _SL_PROP_CLIENT_QUERY_H_
#define _SL_PROP_CLIENT_QUERY_H_

#include <stdint.h>
#include <stdlib.h>

#include <prop-client/prop_spec.h>

/** Maximum number of elements in a query reply */
#define SL_PROP_QUERY_MAX_ELEMENTS	48

/** Default number of data bytes per get/set message */
#define SL_PROP_QUERY_CHUNKSIZE_DEFAULT	200

/** @brief Query types */
enum sl_prop_query_type {
	SL_PROP_QUERY_UNKNOWN = 0,
	SL_PROP_QUERY_GET = 1,
	SL_PROP_QUERY_SET = 2,
};

/** @brief Query reply element */
struct sl_prop_query_element {
	prop_id_t id;
	const void *value;
	size_t size;
};

/** @brief Query state - should not be directly modified */
struct sl_prop_query {
	enum sl_prop_query_type type;
	uint8_t flags;
	uint8_t *txbuf;
	size_t txbufsize;
	uint8_t *rxbuf;
	size_t rxbufsize;
	bool allocated;
	size_t txsize;
	size_t rxsize;
	size_t chunksize;
	struct sl_prop_query_element element[SL_PROP_QUERY_MAX_ELEMENTS];
	size_t elements;
};

/**
 * @brief Create dynamically allocated property query
 *
 * This function initializes a new query and dynamically allocates transmit and
 * receive buffers. The rxsize argument can be set to 0 for pure 'set' queries.
 *
 * @param query Query to initialize.
 * @param txsize Size of transmit buffer in bytes.
 * @param rxsize Size of receive buffer in bytes.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_create(struct sl_prop_query *query, size_t txsize, size_t rxsize);

/**
 * @brief Create statically allocated property query
 *
 * This function initializes a new query using statically allocated transmit
 * and receive buffers. The rxbuf and rxsize argument can be set to NULL and 0
 * for pure 'set' queries.
 *
 * @param query Query to initialize.
 * @param txbuf Pointer to transmit buffer.
 * @param txsize Size of txbuf in bytes.
 * @param rxbuf Pointer to receive buffer.
 * @param rxsize Size of receive buffer in bytes.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_create_static(struct sl_prop_query *query, uint8_t *txbuf, size_t txsize, uint8_t *rxbuf, size_t rxsize);

/**
 * @brief Reset query so it can be reused
 *
 * This function resets a query without the need to reallocate buffers. This
 * allows the query to be reused multiple times.
 *
 * @param query Query to reset.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_reset(struct sl_prop_query *query);

/**
 * @brief Free memory allocated for query
 *
 * Destroy query and free any memory allocated by sl_prop_remote_query_create.
 * The query may not be accessed after this function is called.
 *
 * @param query Query to destroy.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_destroy(struct sl_prop_query *query);

/**
 * @brief Add property get to query
 *
 * This function adds an ID to the list of properties to get in a query. A
 * group ID can be added to fetch a full property group in one request message.
 *
 * @param query Query to add GET field to.
 * @param id ID of property or property group to get.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_get(struct sl_prop_query *query, prop_id_t id);

/**
 * @brief Read reply value from query
 *
 * This function reads the reply value from a GET query. The query must have
 * been successfully sent and replied using sl_prop_remote_query_send.
 *
 * @param query Query to read reply from.
 * @param id ID of property to get.
 * @param value Pointer where result should be stored.
 * @param valsize Size of buffer pointed to by value.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_get_reply(struct sl_prop_query *query, prop_id_t id, void *value, size_t valsize);

/**
 * @brief Read reply value from query as string
 *
 * This function is similar to sl_prop_remote_query_get_reply but outputs the
 * reply value as a string instead of binary.
 *
 * @param query Query to read reply from.
 * @param prop Pointer to the property spec of the property to get.
 * @param string Pointer where string result should be stored.
 * @param size Size of string buffer.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_get_reply_to_string(struct sl_prop_query *query, const struct prop_spec *prop, char *string, size_t size);

/**
 * @brief Add property set to query
 *
 * This function adds an ID and new value to the list of properties to set in a
 * query.
 *
 * @param query Query to add SET field to.
 * @param id ID of property to set.
 * @param value New value to set.
 * @param valsize Size of buffer pointed to by value.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_set(struct sl_prop_query *query, prop_id_t id, const void *value, size_t valsize);

/**
 * @brief Send query and wait for reply
 *
 * Send a property query to a remote system and wait for reply. A query can be
 * sent multiple times, e.g. to read out the same telemetry values repeatedly.
 *
 * @param query Query to send.
 * @param node CSP address of the property node.
 * @param port CSP port of the property node.
 * @param timeout Timeout of the command in milliseconds.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_prop_remote_query_send(struct sl_prop_query *query, uint8_t node, uint8_t port, uint32_t timeout);

#endif /* _SL_PROP_CLIENT_QUERY_H_ */
