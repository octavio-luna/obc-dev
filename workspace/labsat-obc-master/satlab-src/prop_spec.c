/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <prop-client/prop_spec.h>

#include <string.h>
#include <errno.h>

prop_id_t sl_prop_remote_id(const struct prop_spec *spec)
{
	return spec ? spec->id : 0;
}

prop_type_t sl_prop_remote_type(const struct prop_spec *spec)
{
	return spec ? spec->type.type : 0;
}

const char *sl_prop_spec_type_string(const struct prop_spec *prop)
{
	switch (prop->type.type) {
	case PROP_TYPE_BOOL:
		return "bool";
	case PROP_TYPE_UINT8:
		return "u8";
	case PROP_TYPE_UINT16:
		return "u16";
	case PROP_TYPE_UINT32:
		return "u32";
	case PROP_TYPE_UINT64:
		return "u64";
	case PROP_TYPE_INT8:
		return "i8";
	case PROP_TYPE_INT16:
		return "i16";
	case PROP_TYPE_INT32:
		return "i32";
	case PROP_TYPE_INT64:
		return "i64";
	case PROP_TYPE_DOUBLE:
		return "dbl";
	case PROP_TYPE_FLOAT:
		return "flt";
	case PROP_TYPE_STRING:
		return "str";
	case PROP_TYPE_BINARY:
		return "bin";
	default:
		return "?";
	}
}

const struct prop_group_spec *sl_prop_group_spec_find_by_name(const struct prop_subsys_spec *spec, const char *name)
{
	size_t i;
	const struct prop_group_spec *group;

	for (i = 0; i < spec->count; i++) {
		group = &spec->groups[i];
		if (!strcmp(name, group->name))
			return group;
	}

	return NULL;
}

const struct prop_spec *sl_prop_spec_find_by_name(const struct prop_subsys_spec *spec, const char *name)
{
	size_t i, group_len;
	char group_name[50];
	const struct prop_group_spec *group;
	const struct prop_spec *prop;
	const char *prop_name;

	prop_name = strchr(name, '.');
	if (!prop_name)
		return NULL;

	group_len = prop_name - name;
	prop_name++;

	if (group_len > sizeof(group_name) - 1)
		return NULL;

	strncpy(group_name, name, group_len);
	group_name[group_len] = '\0';

	group = sl_prop_group_spec_find_by_name(spec, group_name);
	if (!group)
		return NULL;

	for (i = 0; i < group->count; i++) {
		prop = &group->props[i];
		if (!strcmp(prop_name, prop->name))
			return prop;
	}

	return NULL;
}

int sl_prop_group_spec_iterate(const struct prop_subsys_spec *spec, sl_prop_group_spec_iter_t iter, void *arg)
{
	size_t i;
	const struct prop_group_spec *group;

	for (i = 0; i < spec->count; i++) {
		group = &spec->groups[i];
		iter(group, arg);
	}

	return 0;
}

int sl_prop_spec_iterate(const struct prop_group_spec *group, sl_prop_spec_iter_t iter, void *arg)
{
	size_t i;
	const struct prop_spec *prop;

	for (i = 0; i < group->count; i++) {
		prop = &group->props[i];
		iter(prop, arg);
	}

	return 0;
}
