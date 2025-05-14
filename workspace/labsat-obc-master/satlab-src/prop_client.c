/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <prop-client/prop_client.h>

#include <stdint.h>
// #include <stdio.h> not to be used directly in this project over freertos.

#include <csp/csp.h>
#include <endian.h>

#include <prop-client/prop_proto.h>

#include <LogManager.h>

static int sl_prop_error_to_errno(uint8_t error) {
	switch (error) {
	case SL_PROP_ERR_NONE:		/* No error */
		return 0;
	case SL_PROP_ERR_INVAL:		/* Invalid argument */
		return -EINVAL;
	case SL_PROP_ERR_NOENT:		/* No such file or directory */
		return -ENOENT;
	case SL_PROP_ERR_NOSPC:		/* No space left on device */
		return -ENOSPC;
	case SL_PROP_ERR_IO:		/* I/O error */
		return -EIO;
	case SL_PROP_ERR_TIMEDOUT:	/* Timeout while waiting for data */
		return -ETIMEDOUT;
	default:			/* Assume I/O error for everything else */
		return -EIO;
	}
}

int sl_prop_parse_bool(const char *string, size_t size, uint8_t *value)
{
	if (!strcmp(string, "1") ||
	    !strcmp(string, "true") ||
	    !strcmp(string, "yes")) {
		*value = 1;
		return 0;
	} else if (!strcmp(string, "0") ||
	    !strcmp(string, "false") ||
	    !strcmp(string, "no")) {
		*value = 0;
		return 0;
	}

	return -EINVAL;
}

int sl_prop_parse_binary(const char *string, size_t size, uint8_t *bin, size_t binsize)
{
	size_t i;
	uint8_t byte;
	char bytestr[3];
	char *endptr;

	/* Binary properties must be specified in full (Note: size includes trailing zero) */
	if (size - 1 != 2 * binsize)
		return -EINVAL;

	for (i = 0; i < (size - 1) / 2; i++) {
		bytestr[0] = string[i * 2];
		bytestr[1] = string[i * 2 + 1];
		bytestr[2] = '\0';
		byte = (uint8_t)strtoul(bytestr, &endptr, 16);
		if (*endptr != '\0')
			return -EINVAL;

		bin[i] = byte;
	}

	return 0;
}

int sl_prop_remote_get(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, void *value, size_t size)
{
	int ret;
	struct sl_prop_query query;
	uint8_t txbuf[128], rxbuf[128];

	ret = sl_prop_remote_query_create_static(&query, txbuf, sizeof(txbuf), rxbuf, sizeof(rxbuf));
	if (ret < 0)
		return ret;

	ret = sl_prop_remote_query_get(&query, id);
	if (ret < 0)
		goto out;

	ret = sl_prop_remote_query_send(&query, node, port, timeout);
	if (ret < 0)
		goto out;

	ret = sl_prop_remote_query_get_reply(&query, id, value, size);
	if (ret < 0)
		goto out;

out:
	sl_prop_remote_query_destroy(&query);

	return ret;
}

int sl_prop_remote_get_to_string(uint8_t node, uint8_t port, uint32_t timeout, const struct prop_spec *spec, char *string, size_t size)
{
	int ret;
	struct sl_prop_query query;
	uint8_t txbuf[128], rxbuf[128];

	ret = sl_prop_remote_query_create_static(&query, txbuf, sizeof(txbuf), rxbuf, sizeof(rxbuf));
	if (ret < 0)
		return ret;

	ret = sl_prop_remote_query_get(&query, spec->id);
	if (ret < 0)
		goto out;

	ret = sl_prop_remote_query_send(&query, node, port, timeout);
	if (ret < 0)
		goto out;

	ret = sl_prop_remote_query_get_reply_to_string(&query, spec, string, size);
	if (ret < 0)
		goto out;

out:
	sl_prop_remote_query_destroy(&query);

	return ret;
}

int sl_prop_remote_set(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, const void *value, size_t size)
{
	int ret;
	struct sl_prop_query query;
	uint8_t txbuf[128];

	ret = sl_prop_remote_query_create_static(&query, txbuf, sizeof(txbuf), NULL, 0);
	if (ret < 0)
		return ret;

	ret = sl_prop_remote_query_set(&query, id, value, size);
	if (ret < 0) {
		sl_prop_remote_query_destroy(&query);
		return ret;
	}

	ret = sl_prop_remote_query_send(&query, node, port, timeout);

	sl_prop_remote_query_destroy(&query);

	return ret;
}

int sl_prop_remote_set_from_string(uint8_t node, uint8_t port, uint32_t timeout, const struct prop_spec *spec, const char *string, size_t size)
{
	int ret;

	const char *fmt;

	switch (spec->type.type) {
	case PROP_TYPE_BOOL: {
		uint8_t boolval;
		if (sl_prop_parse_bool(string, size, &boolval) < 0)
			return -EINVAL;
		ret = sl_prop_remote_set_bool(node, port, timeout, spec->id, boolval);
		break;
	}
	case PROP_TYPE_UINT8: {
		uint8_t u8val;
		fmt = spec->type.fmt ? spec->type.fmt : "%"PRIu8;
		if (sscanf(string, fmt, &u8val) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_uint8(node, port, timeout, spec->id, u8val);
		break;
	}
	case PROP_TYPE_UINT16: {
		uint16_t u16val;
		fmt = spec->type.fmt ? spec->type.fmt : "%"PRIu16;
		if (sscanf(string, fmt, &u16val) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_uint16(node, port, timeout, spec->id, u16val);
		break;
	}
	case PROP_TYPE_UINT32: {
		uint32_t u32val;
		fmt = spec->type.fmt ? spec->type.fmt : "%"PRIu32;
		if (sscanf(string, fmt, &u32val) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_uint32(node, port, timeout, spec->id, u32val);
		break;
	}
	case PROP_TYPE_UINT64: {
		uint64_t u64val;
		fmt = spec->type.fmt ? spec->type.fmt : "%"PRIu64;
		if (sscanf(string, fmt, &u64val) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_uint64(node, port, timeout, spec->id, u64val);
		break;
	}
	case PROP_TYPE_INT8: {
		int8_t i8val;
		fmt = spec->type.fmt ? spec->type.fmt : "%"PRIi8;
		if (sscanf(string, fmt, &i8val) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_int8(node, port, timeout, spec->id, i8val);
		break;
	}
	case PROP_TYPE_INT16: {
		int16_t i16val;
		fmt = spec->type.fmt ? spec->type.fmt : "%"PRIi16;
		if (sscanf(string, fmt, &i16val) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_int16(node, port, timeout, spec->id, i16val);
		break;
	}
	case PROP_TYPE_INT32: {
		int32_t i32val;
		fmt = spec->type.fmt ? spec->type.fmt : "%"PRIi32;
		if (sscanf(string, fmt, &i32val) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_int32(node, port, timeout, spec->id, i32val);
		break;
	}
	case PROP_TYPE_INT64: {
		int64_t i64val;
		fmt = spec->type.fmt ? spec->type.fmt : "%"PRIi64;
		if (sscanf(string, fmt, &i64val) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_int64(node, port, timeout, spec->id, i64val);
		break;
	}
	case PROP_TYPE_DOUBLE: {
		double doubleval;
		if (sscanf(string, "%lf", &doubleval) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_double(node, port, timeout, spec->id, doubleval);
		break;
	}
	case PROP_TYPE_FLOAT: {
		float floatval;
		if (sscanf(string, "%f", &floatval) != 1)
			return -EINVAL;
		ret = sl_prop_remote_set_float(node, port, timeout, spec->id, floatval);
		break;
	}
	case PROP_TYPE_STRING: {
		if (size > spec->size)
			return -ENOSPC;
		ret = sl_prop_remote_set_string(node, port, timeout, spec->id, string, size);
		break;
	}
	case PROP_TYPE_BINARY: {
		uint8_t bin[128];
		if (spec->size > sizeof(bin))
			return -ENOSPC;
		if (sl_prop_parse_binary(string, size, bin, spec->size) < 0)
			return -EINVAL;
		ret = sl_prop_remote_set_binary(node, port, timeout, spec->id, bin, spec->size);
		break;
	}
	default:
		return -EINVAL;
	}

	return ret;
}

int sl_prop_remote_reset(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id)
{
	int ret;
	sl_prop_reset_req_t request;
	sl_prop_reset_rep_t reply;

	request.type = SL_PROP_RESET_REQUEST;
	request.flags = 0;
	request.id = htobe16(id);

	ret = csp_transaction(CSP_PRIO_NORM, node, port, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_PROP_RESET_REPLY)
		return -EINVAL;

	if (reply.error != SL_PROP_ERR_NONE)
		return sl_prop_error_to_errno(reply.error);

	return 0;
}

int sl_prop_remote_save(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback)
{
	int ret;
	sl_prop_save_req_t request;
	sl_prop_save_rep_t reply;
	uint32_t key;

	key = fallback ? SL_PROP_SAVE_FALLBACK_KEY : SL_PROP_SAVE_BOOT_KEY;

	request.type = SL_PROP_SAVE_REQUEST;
	request.flags = 0;
	request.id = htobe16(group);
	request.key = htobe32(key);

	ret = csp_transaction(CSP_PRIO_NORM, node, port, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_PROP_SAVE_REPLY)
		return -EINVAL;

	if (reply.error != SL_PROP_ERR_NONE)
		return sl_prop_error_to_errno(reply.error);

	return 0;
}

int sl_prop_remote_load(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback)
{
	int ret;
	sl_prop_load_req_t request;
	sl_prop_load_rep_t reply;
	uint32_t key;

	key = fallback ? SL_PROP_LOAD_FALLBACK_KEY : SL_PROP_LOAD_BOOT_KEY;

	request.type = SL_PROP_LOAD_REQUEST;
	request.flags = 0;
	request.id = htobe16(group);
	request.key = htobe32(key);

	ret = csp_transaction(CSP_PRIO_NORM, node, port, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_PROP_LOAD_REPLY)
		return -EINVAL;

	if (reply.error != SL_PROP_ERR_NONE)
		return sl_prop_error_to_errno(reply.error);

	return 0;
}

int sl_prop_remote_erase(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback)
{
	int ret;
	sl_prop_erase_req_t request;
	sl_prop_erase_rep_t reply;
	uint32_t key;

	key = fallback ? SL_PROP_ERASE_FALLBACK_KEY : SL_PROP_ERASE_BOOT_KEY;

	request.type = SL_PROP_ERASE_REQUEST;
	request.flags = 0;
	request.id = htobe16(group);
	request.key = htobe32(key);

	ret = csp_transaction(CSP_PRIO_NORM, node, port, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_PROP_ERASE_REPLY)
		return -EINVAL;

	if (reply.error != SL_PROP_ERR_NONE)
		return sl_prop_error_to_errno(reply.error);

	return 0;
}

int sl_prop_remote_lock(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback)
{
	int ret;
	sl_prop_lock_req_t request;
	sl_prop_lock_rep_t reply;
	uint32_t key;

	key = fallback ? SL_PROP_LOCK_FALLBACK_KEY : SL_PROP_LOCK_BOOT_KEY;

	request.type = SL_PROP_LOCK_REQUEST;
	request.flags = 0;
	request.id = htobe16(group);
	request.key = htobe32(key);

	ret = csp_transaction(CSP_PRIO_NORM, node, port , timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_PROP_LOCK_REPLY)
		return -EINVAL;

	if (reply.error != SL_PROP_ERR_NONE)
		return sl_prop_error_to_errno(reply.error);

	return 0;
}

int sl_prop_remote_unlock(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t group, bool fallback)
{
	int ret;
	sl_prop_unlock_req_t request;
	sl_prop_unlock_rep_t reply;
	uint32_t key;

	key = fallback ? SL_PROP_UNLOCK_FALLBACK_KEY : SL_PROP_UNLOCK_BOOT_KEY;

	request.type = SL_PROP_UNLOCK_REQUEST;
	request.flags = 0;
	request.id = htobe16(group);
	request.key = htobe32(key);

	ret = csp_transaction(CSP_PRIO_NORM, node, port, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_PROP_UNLOCK_REPLY)
		return -EINVAL;

	if (reply.error != SL_PROP_ERR_NONE)
		return sl_prop_error_to_errno(reply.error);

	return 0;
}
