/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <prop-client/prop_client_helpers.h>
#include <prop-client/prop_client.h>

int sl_prop_remote_set_bool(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, bool val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(uint8_t));
}

int sl_prop_remote_get_bool(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, bool *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(uint8_t));
}

int sl_prop_remote_set_int8(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int8_t val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_int8(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int8_t *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_int16(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int16_t val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_int16(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int16_t *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_int32(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int32_t val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_int32(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int32_t *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_int64(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int64_t val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_int64(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, int64_t *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_uint8(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint8_t val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_uint8(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint8_t *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_uint16(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint16_t val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_uint16(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint16_t *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_uint32(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint32_t val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_uint32(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint32_t *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_uint64(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint64_t val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_uint64(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint64_t *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_float(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, float val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_float(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, float *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_double(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, double val)
{
	return sl_prop_remote_set(node, port, timeout, id, &val, sizeof(val));
}

int sl_prop_remote_get_double(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, double *val)
{
	return sl_prop_remote_get(node, port, timeout, id, val, sizeof(*val));
}

int sl_prop_remote_set_string(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, const char *val, size_t size)
{
	return sl_prop_remote_set(node, port, timeout, id, val, size);
}

int sl_prop_remote_get_string(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, char *val, size_t size)
{
	return sl_prop_remote_get(node, port, timeout, id, val, size);
}

int sl_prop_remote_set_binary(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, const uint8_t *val, size_t size)
{
	return sl_prop_remote_set(node, port, timeout, id, val, size);
}

int sl_prop_remote_get_binary(uint8_t node, uint8_t port, uint32_t timeout, prop_id_t id, uint8_t *val, size_t size)
{
	return sl_prop_remote_get(node, port, timeout, id, val, size);
}
