/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <prop-client/prop_query.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h> // using our special implementation of stdio.c
#include <errno.h>

#include <csp/csp.h>
#include <endian.h>

#include <prop-client/prop_client.h>
#include <prop-client/prop_proto.h>

float big_endian_to_host_float(float big_endian_value) {
    uint32_t temp;
    memcpy(&temp, &big_endian_value, sizeof(temp)); // Reinterpret the float as uint32_t
    temp = be32toh(temp); // Convert from big-endian to host-endian
    float result;
    memcpy(&result, &temp, sizeof(result)); // Reinterpret the uint32_t back as float
    return result;
}

double big_endian_to_host_double(double big_endian_value) {
    uint64_t temp;
    memcpy(&temp, &big_endian_value, sizeof(temp)); // Reinterpret the double as uint64_t
    temp = be64toh(temp); // Convert from big-endian to host-endian
    double result;
    memcpy(&result, &temp, sizeof(result)); // Reinterpret the uint64_t back as double
    return result;
}

float host_to_big_endian_float(float host_value) {
    uint32_t temp;
    memcpy(&temp, &host_value, sizeof(temp)); // Reinterpret the float as uint32_t
    temp = htobe32(temp); // Convert from host-endian to big-endian
    float result;
    memcpy(&result, &temp, sizeof(result)); // Reinterpret the uint32_t back as float
    return result;
}

double host_to_big_endian_double(double host_value) {
    uint64_t temp;
    memcpy(&temp, &host_value, sizeof(temp)); // Reinterpret the double as uint64_t
    temp = htobe64(temp); // Convert from host-endian to big-endian
    double result;
    memcpy(&result, &temp, sizeof(result)); // Reinterpret the uint64_t back as double
    return result;
}

static int sl_prop_remote_query_unpack_value(prop_id_t id, const void *buf, size_t bufsize, uint8_t *value, size_t valsize)
{
	const void *unpack_value = NULL;
	size_t unpack_size = 0, consumed = 0;

	uint16_t u16val;
	uint32_t u32val;
	uint64_t u64val;
	float floatval;
	double doubleval;

	uint16_t varsize = 0;

	prop_type_t type = PROP_TYPE(id);

	switch (type) {
	case PROP_TYPE_BOOL:
	case PROP_TYPE_INT8:
	case PROP_TYPE_UINT8:
		unpack_value = buf;
		unpack_size = sizeof(uint8_t);
		break;
	case PROP_TYPE_INT16:
	case PROP_TYPE_UINT16:
		unpack_value = &u16val;
		unpack_size = sizeof(u16val);
		if (bufsize < unpack_size)
			return -ENOMEM;
		memcpy(&u16val, buf, unpack_size);
		u16val = be16toh(u16val);
		break;
	case PROP_TYPE_INT32:
	case PROP_TYPE_UINT32:
		unpack_value = &u32val;
		unpack_size = sizeof(u32val);
		if (bufsize < unpack_size)
			return -ENOMEM;
		memcpy(&u32val, buf, unpack_size);
		u32val = be32toh(u32val);
		break;
	case PROP_TYPE_INT64:
	case PROP_TYPE_UINT64:
		unpack_value = &u64val;
		unpack_size = sizeof(u64val);
		if (bufsize < unpack_size)
			return -ENOMEM;
		memcpy(&u64val, buf, unpack_size);
		u64val = be64toh(u64val);
		break;
	case PROP_TYPE_FLOAT:
		unpack_value = &floatval;
		unpack_size = sizeof(floatval);
		if (bufsize < unpack_size)
			return -ENOMEM;
		memcpy(&floatval, buf, unpack_size);
		floatval = big_endian_to_host_float(floatval); 
		break;
	case PROP_TYPE_DOUBLE:
		unpack_value = &doubleval;
		unpack_size = sizeof(doubleval);
		if (bufsize < unpack_size)
			return -ENOMEM;
		memcpy(&doubleval, buf, unpack_size);
		doubleval = big_endian_to_host_double(doubleval);
		break;
	case PROP_TYPE_STRING:
	case PROP_TYPE_BINARY:
		/* Unpack variable length field */
		if (bufsize < sizeof(varsize))
			return -EINVAL;
		memcpy(&varsize, buf, sizeof(varsize));
		varsize = be16toh(varsize);
		consumed += sizeof(varsize);

		unpack_value = buf + sizeof(varsize);
		unpack_size = varsize;
		break;
	case PROP_TYPE_ARRAY:
		return -ENOSYS;
	default:
		return -EINVAL;
	}

	consumed += unpack_size;

	/* Copy value if caller requested it */
	if (value && valsize && unpack_value && unpack_size) {
		/* And that we don't overrun value buffer */
		if (unpack_size != valsize)
			return -ENOSPC;
		memcpy(value, unpack_value, unpack_size);
	}

	return consumed;
}

static int sl_prop_remote_query_pack_value(prop_id_t id, void *buf, size_t bufsize, const uint8_t *value, size_t valsize)
{
	const void *pack_value = NULL;
	size_t pack_size = 0, consumed = 0;

	uint16_t u16val;
	uint32_t u32val;
	uint64_t u64val;
	float floatval;
	double doubleval;

	uint16_t varsize;

	prop_type_t type = PROP_TYPE(id);

	switch (type) {
	case PROP_TYPE_BOOL:
	case PROP_TYPE_INT8:
	case PROP_TYPE_UINT8:
		pack_value = value;
		pack_size = sizeof(uint8_t);
		break;
	case PROP_TYPE_INT16:
	case PROP_TYPE_UINT16:
		pack_value = &u16val;
		pack_size = sizeof(u16val);
		if (valsize != pack_size)
			return -ENOMEM;
		memcpy(&u16val, value, pack_size);
		u16val = htobe16(u16val);
		break;
	case PROP_TYPE_INT32:
	case PROP_TYPE_UINT32:
		pack_value = &u32val;
		pack_size = sizeof(u32val);
		if (valsize != pack_size)
			return -ENOMEM;
		memcpy(&u32val, value, pack_size);
		u32val = htobe32(u32val);
		break;
	case PROP_TYPE_INT64:
	case PROP_TYPE_UINT64:
		pack_value = &u64val;
		pack_size = sizeof(u64val);
		if (valsize != pack_size)
			return -ENOMEM;
		memcpy(&u64val, value, pack_size);
		u64val = htobe64(u64val);
		break;
	case PROP_TYPE_FLOAT:
		pack_value = &floatval;
		pack_size = sizeof(floatval);
		if (valsize != pack_size)
			return -ENOMEM;
		memcpy(&floatval, value, pack_size);
		floatval = host_to_big_endian_float(floatval);
		break;
	case PROP_TYPE_DOUBLE:
		pack_value = &doubleval;
		pack_size = sizeof(doubleval);
		if (valsize != pack_size)
			return -ENOMEM;
		memcpy(&doubleval, value, pack_size);
		doubleval = host_to_big_endian_double(doubleval);
		break;
	case PROP_TYPE_STRING:
	case PROP_TYPE_BINARY:
		/* Pack variable length field */
		varsize = htobe16(valsize);
		memcpy(buf, &varsize, sizeof(varsize));
		buf += sizeof(varsize);
		bufsize -= sizeof(varsize);
		consumed += sizeof(varsize);

		pack_value = value;
		pack_size = valsize;
		break;
	case PROP_TYPE_ARRAY:
		return -ENOSYS;
	default:
		return -EINVAL;
	}

	/* Verify that we don't overrun buffer */
	if (valsize < pack_size)
		return -ENOMEM;

	/* Copy value to buffer */
	memcpy(buf, pack_value, pack_size);

	/* Return number of consumed bytes */
	consumed += pack_size;

	return consumed;
}

static int sl_prop_remote_query_unpack_buffer(struct sl_prop_query *query, const uint8_t *buf, size_t bufsize)
{
	int ret;
	size_t parsed = 0;
	uint16_t id;

	query->elements = 0;

	while (parsed < bufsize) {
		/* Verify available space in reply element list */
		if (query->elements + 1 > sizeof(query->element)/sizeof(query->element[0]))
			return -ENOSPC;

		/* Verify remaining bytes in request buffer */
		if (bufsize - parsed < sizeof(id))
			return -EINVAL;

		/* Read out ID */
		memcpy(&id, &buf[parsed], sizeof(id));
		parsed += sizeof(id);
		id = be16toh(id);

		/* Read out or skip value */
		ret = sl_prop_remote_query_unpack_value(id, &buf[parsed], bufsize - parsed, NULL, 0);
		if (ret < 0)
			return ret;

		/* Add to reply list */
		query->element[query->elements].id = id;
		query->element[query->elements].value = &buf[parsed];
		query->element[query->elements].size = ret;
		query->elements++;

		/* Proceed to next ID */
		parsed += ret;
	}

	return 0;
}

int sl_prop_remote_query_create_static(struct sl_prop_query *query, uint8_t *txbuf, size_t txsize, uint8_t *rxbuf, size_t rxsize)
{
	int ret;

	query->txbuf = txbuf;
	query->txbufsize = txsize;
	query->rxbuf = rxbuf;
	query->rxbufsize = rxsize;
	query->allocated = false;

	ret = sl_prop_remote_query_reset(query);
	if (ret < 0) {
		sl_prop_remote_query_destroy(query);
		return ret;
	}

	return 0;
}

int sl_prop_remote_query_create(struct sl_prop_query *query, size_t txsize, size_t rxsize)
{
	int ret;
	uint8_t *buffers;

	buffers = pvPortMalloc(txsize + rxsize);
	if (!buffers)
		return -ENOMEM;

	ret = sl_prop_remote_query_create_static(query, buffers, txsize, buffers + txsize, rxsize);
	if (ret < 0) {
		vPortFree(buffers);
		return ret;
	}

	query->allocated = true;

	return 0;
}

int sl_prop_remote_query_reset(struct sl_prop_query *query)
{
	if (!query)
		return -EINVAL;

	query->type = SL_PROP_QUERY_UNKNOWN;
	query->txsize = 0;
	query->rxsize = 0;
	query->elements = 0;
	query->chunksize = SL_PROP_QUERY_CHUNKSIZE_DEFAULT;
	query->flags = 0;

	return 0;
}

int sl_prop_remote_query_destroy(struct sl_prop_query *query)
{
	if (query->allocated) {
		vPortFree(query->txbuf);
		query->allocated = false;
	}

	query->txbuf = NULL;
	query->rxbuf = NULL;

	return 0;
}

static int sl_prop_remote_query_send_get(struct sl_prop_query *query, uint8_t node, uint8_t port, uint32_t timeout)
{
	sl_prop_get_req_t *request;
	sl_prop_get_rep_t *reply;
	csp_conn_t *conn;
	csp_packet_t *packet;
	size_t sent = 0, received = 0, datasize, remain;

	conn = csp_connect(CSP_PRIO_NORM, node, port, timeout, CSP_O_NONE);
	if (!conn)
		return -ECONNREFUSED;

	remain = (query->txsize + query->chunksize - 1) / query->chunksize;

	while (sent < query->txsize) {
		if (query->txsize - sent > query->chunksize)
			datasize = query->chunksize;
		else
			datasize = query->txsize - sent;
		packet = csp_buffer_get(sizeof(*request) + datasize);
		if (!packet) {
			csp_close(conn);
			return -ENOMEM;
		}

		request = (sl_prop_get_req_t *)packet->data;
		request->type = SL_PROP_GET_REQUEST;
		request->flags = (!sent ? SL_PROP_FLAG_BEGIN : 0) | query->flags;
		request->chunksize = query->chunksize;
		request->remain = --remain;

		memcpy(request->data, &query->txbuf[sent], datasize);
		packet->length = sizeof(*request) + datasize;

		csp_send(conn, packet);
		
		sent += datasize;
	}

	query->rxsize = query->rxbufsize;
	while (received < query->rxsize) {
		packet = csp_read(conn, timeout);
		if (!packet) {
			csp_close(conn);
			return -ETIMEDOUT;
		}

		reply = (sl_prop_get_rep_t *)packet->data;
		if (reply->type != SL_PROP_GET_REPLY) {
			csp_buffer_free(packet);
			csp_close(conn);
			return -EINVAL;
		}

		if (reply->error != SL_PROP_ERR_NONE) {
			csp_buffer_free(packet);
			csp_close(conn);
			return -reply->error;
		}

		remain = reply->remain;

		datasize = packet->length - sizeof(*reply);
		if (received + datasize > query->rxsize) {
			csp_buffer_free(packet);
			csp_close(conn);
			return -ENOSPC;
		}

		memcpy(&query->rxbuf[received], reply->data, datasize);

		csp_buffer_free(packet);

		received += datasize;

		if (remain == 0)
			break;
	}

	csp_close(conn);

	/* Unpack buffer */
	query->rxsize = received;
	return sl_prop_remote_query_unpack_buffer(query, query->rxbuf, query->rxsize);
}

static int sl_prop_remote_query_send_set(struct sl_prop_query *query, uint8_t node, uint8_t port, uint32_t timeout)
{
	sl_prop_set_req_t *request;
	sl_prop_set_rep_t *reply;
	csp_conn_t *conn;
	csp_packet_t *packet;
	size_t sent = 0, datasize, remain;

	conn = csp_connect(CSP_PRIO_NORM, node, port, timeout, CSP_O_NONE);
	if (!conn)
		return -ECONNREFUSED;

	remain = (query->txsize + query->chunksize - 1) / query->chunksize;

	while (sent < query->txsize) {
		if (query->txsize - sent > query->chunksize)
			datasize = query->chunksize;
		else
			datasize = query->txsize - sent;
		packet = csp_buffer_get(sizeof(*request) + datasize);
		if (!packet) {
			csp_close(conn);
			return -ENOMEM;
		}

		request = (sl_prop_set_req_t *)packet->data;
		request->type = SL_PROP_SET_REQUEST;
		request->flags = (!sent ? SL_PROP_FLAG_BEGIN : 0) | query->flags;
		request->remain = --remain;

		memcpy(request->data, &query->txbuf[sent], datasize);
		packet->length = sizeof(*request) + datasize;

		csp_send(conn, packet);

		sent += datasize;
	}

	packet = csp_read(conn, timeout);
	if (!packet) {
		csp_close(conn);
		return -ETIMEDOUT;
	}

	reply = (sl_prop_set_rep_t *)packet->data;
	if (reply->type != SL_PROP_SET_REPLY) {
		csp_buffer_free(packet);
		csp_close(conn);
		return -EINVAL;
	}

	if (reply->error != SL_PROP_ERR_NONE) {
		csp_buffer_free(packet);
		csp_close(conn);
		return -reply->error;
	}

	csp_buffer_free(packet);

	csp_close(conn);

	return 0;
}

int sl_prop_remote_query_send(struct sl_prop_query *query, uint8_t node, uint8_t port, uint32_t timeout)
{
	if (!query)
		return -EINVAL;

	if (query->type == SL_PROP_QUERY_GET)
		return sl_prop_remote_query_send_get(query, node, port, timeout);
	else if (query->type == SL_PROP_QUERY_SET)
		return sl_prop_remote_query_send_set(query, node, port, timeout);
	else
		return -EINVAL;
}

int sl_prop_remote_query_get(struct sl_prop_query *query, prop_id_t id)
{
	if (!query)
		return -EINVAL;

	if (query->type != SL_PROP_QUERY_UNKNOWN && query->type != SL_PROP_QUERY_GET)
		return -EINVAL;

	if (query->txsize + sizeof(id) > query->txbufsize)
		return -ENOSPC;

	query->type = SL_PROP_QUERY_GET;

	/* Add ID to pack buffer */
	id = htobe16(id);
	memcpy(&query->txbuf[query->txsize], &id, sizeof(id));
	query->txsize += sizeof(id);

	return 0;
}

int sl_prop_remote_query_get_reply(struct sl_prop_query *query, prop_id_t id, void *value, size_t valsize)
{
	size_t i;
	struct sl_prop_query_element *e;

	if (!query)
		return -EINVAL;

	if (query->type != SL_PROP_QUERY_GET)
		return -EINVAL;

	for (i = 0; i < query->elements; i++) {
		e = &query->element[i];
		if (e->id != id)
			continue;

		return sl_prop_remote_query_unpack_value(e->id, e->value, e->size, value, valsize);
	}

	return -ENOENT;
}

int sl_prop_remote_query_get_reply_to_string(struct sl_prop_query *query, const struct prop_spec *prop, char *string, size_t size)
{
	int ret = -EINVAL;

	const char *fmt;
	prop_type_t type = prop->type.type;

	switch (type) {
	case PROP_TYPE_BOOL: {
		uint8_t boolval;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &boolval, sizeof(boolval));
		if (ret < 0)
			break;
		ret = snprintf(string, size, boolval ? "true" : "false");
		break;
	}
	case PROP_TYPE_UINT8: {
		uint8_t u8val;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &u8val, sizeof(u8val));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%"PRIu8;
		ret = snprintf(string, size, fmt, u8val);
		break;
	}
	case PROP_TYPE_UINT16: {
		uint16_t u16val;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &u16val, sizeof(u16val));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%"PRIu16;
		ret = snprintf(string, size, fmt, u16val);
		break;
	}
	case PROP_TYPE_UINT32: {
		uint32_t u32val;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &u32val, sizeof(u32val));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%"PRIu32;
		ret = snprintf(string, size, fmt, u32val);
		break;
	}
	case PROP_TYPE_UINT64: {
		uint64_t u64val;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &u64val, sizeof(u64val));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%"PRIu64;
		ret = snprintf(string, size, fmt, u64val);
		break;
	}
	case PROP_TYPE_INT8: {
		int8_t i8val;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &i8val, sizeof(i8val));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%"PRIi8;
		ret = snprintf(string, size, fmt, i8val);
		break;
	}
	case PROP_TYPE_INT16: {
		int16_t i16val;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &i16val, sizeof(i16val));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%"PRIi16;
		ret = snprintf(string, size, fmt, i16val);
		break;
	}
	case PROP_TYPE_INT32: {
		int32_t i32val;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &i32val, sizeof(i32val));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%"PRIi32;
		ret = snprintf(string, size, fmt, i32val);
		break;
	}
	case PROP_TYPE_INT64: {
		int64_t i64val;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &i64val, sizeof(i64val));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%"PRIi64;
		ret = snprintf(string, size, fmt, i64val);
		break;
	}
	case PROP_TYPE_DOUBLE: {
		double doubleval;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &doubleval, sizeof(doubleval));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%f";
		ret = snprintf(string, size, fmt, doubleval);
		break;
	}
	case PROP_TYPE_FLOAT: {
		float floatval;
		ret = sl_prop_remote_query_get_reply(query, prop->id, &floatval, sizeof(floatval));
		if (ret < 0)
			break;
		fmt = prop->type.fmt ? prop->type.fmt : "%f";
		ret = snprintf(string, size, fmt, floatval);
		break;
	}
	case PROP_TYPE_STRING: {
		char tempstring[128];
		if (prop->size > sizeof(tempstring) + 1)
			return -ENOSPC;
		ret = sl_prop_remote_query_get_reply(query, prop->id, tempstring, prop->size);
		if (ret < 0)
			break;
		tempstring[prop->size - 1] = '\0';
		ret = snprintf(string, size, "%s", tempstring);
		break;
	}
	case PROP_TYPE_BINARY: {
		size_t i, offset = 0;
		char bin[128];
		if (prop->size > sizeof(bin))
			return -ENOSPC;
		ret = sl_prop_remote_query_get_reply(query, prop->id, bin, prop->size);
		if (ret < 0)
			break;

		for (i = 0; i < prop->size; i++) {
			if (offset >= size)
				return -EINVAL;
			ret += snprintf(string + offset, size - offset, "%02hhx", bin[i]);
			offset += 2;
		}
		break;
	}
	default:
		return -ENOSYS;
	}

	string[size - 1] = '\0';

	return ret;
}

int sl_prop_remote_query_set(struct sl_prop_query *query, prop_id_t id, const void *value, size_t valsize)
{
	int ret;
	uint16_t id_nbo;

	if (!query)
		return -EINVAL;

	if (query->type != SL_PROP_QUERY_UNKNOWN && query->type != SL_PROP_QUERY_SET)
		return -EINVAL;

	if (query->txsize + sizeof(id) + valsize > query->txbufsize)
		return -ENOSPC;

	query->type = SL_PROP_QUERY_SET;

	/* Add ID to pack buffer */
	id_nbo = htobe16(id);
	memcpy(&query->txbuf[query->txsize], &id_nbo, sizeof(id_nbo));
	query->txsize += sizeof(id_nbo);

	/* And pack new value */
	ret = sl_prop_remote_query_pack_value(id, &query->txbuf[query->txsize], query->txbufsize - query->txsize, value, valsize);
	if (ret < 0)
		return ret;
	query->txsize += ret;

	return 0;
}
