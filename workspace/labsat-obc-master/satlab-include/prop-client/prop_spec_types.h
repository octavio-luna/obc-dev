/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file
 *
 * This file defines the types used by the property system. The macros in this
 * file are used to define subsystem properties, and should not be used outside
 * of the property client library.
 */

#ifndef _SL_PROP_SPEC_TYPES_H_
#define _SL_PROP_SPEC_TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>

/** Properties are identified using a 16-bit ID */
typedef uint16_t prop_id_t;

/** Number of bits for property type */
#define PROP_ID_TYPE_BITS	4

/** Number of bits for property group */
#define PROP_ID_GROUP_BITS	5

/** Number of bits for properties in each group */
#define PROP_ID_PROP_BITS	7

/** Generate a property ID */
#define PROP_MKID(_type, _group, _prop)	\
	((((_type)  & ((1U << PROP_ID_TYPE_BITS)  - 1)) << (PROP_ID_GROUP_BITS + PROP_ID_PROP_BITS)) | \
	 (((_group) & ((1U << PROP_ID_GROUP_BITS) - 1)) << (PROP_ID_PROP_BITS)) | \
	  ((_prop)  & ((1U << PROP_ID_PROP_BITS)  - 1)))

/** Generate a property group ID */
#define PROP_MKGROUPID(_group)	PROP_MKID(0, (_group), 0)

/** Extract group ID from property ID */
#define PROP_GROUPID(_id)	((_id) & PROP_MKID(0, ((1U << PROP_ID_GROUP_BITS) - 1), 0))

/** Extract property type from property ID */
#define PROP_TYPE(_id)		(((_id) & PROP_MKID(((1U << PROP_ID_TYPE_BITS) - 1), 0, 0)) >> (PROP_ID_GROUP_BITS + PROP_ID_PROP_BITS))

/** Return true if property ID is a group ID, false otherwise */
#define PROP_ID_IS_GROUP(_id)	(PROP_GROUPID(_id) == (_id))

/** Generate a boolean array property */
#define PROP_BOOL_ARRAY(_name, _id, _flags, _deflt, _count) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_BOOL, \
			.size = sizeof(uint8_t) \
		}, \
		.count = _count, \
		.size = _count * sizeof(uint8_t), \
		.deflt = _deflt, \
	}

/** Generate a single boolean property */
#define PROP_BOOL(_name, _id, _flags, _deflt) \
	PROP_BOOL_ARRAY(_name, _id, _flags, (const uint8_t []){_deflt}, 1)

/** Generate an unsigned 8-bit array property */
#define PROP_UINT8_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_UINT8, \
			.size = sizeof(uint8_t), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_uint8 = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(uint8_t), \
		.deflt = _deflt, \
	}

/** Generate a single unsigned 8-bit property */
#define PROP_UINT8(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_UINT8_ARRAY(_name, _id, _flags, (const uint8_t []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate an unsigned 16-bit array property */
#define PROP_UINT16_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_UINT16, \
			.size = sizeof(uint16_t), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_uint16 = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(uint16_t), \
		.deflt = _deflt, \
	}

/** Generate a single unsigned 16-bit property */
#define PROP_UINT16(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_UINT16_ARRAY(_name, _id, _flags, (const uint16_t []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate an unsigned 32-bit array property */
#define PROP_UINT32_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_UINT32, \
			.size = sizeof(uint32_t), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_uint32 = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(uint32_t), \
		.deflt = _deflt, \
	}

/** Generate a single unsigned 32-bit property */
#define PROP_UINT32(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_UINT32_ARRAY(_name, _id, _flags, (const uint32_t []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate an unsigned 64-bit array property */
#define PROP_UINT64_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_UINT64, \
			.size = sizeof(uint64_t), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_uint64 = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(uint64_t), \
		.deflt = _deflt, \
	}

/** Generate a single unsigned 64-bit property */
#define PROP_UINT64(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_UINT64_ARRAY(_name, _id, _flags, (const uint64_t []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate a signed 8-bit array property */
#define PROP_INT8_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_INT8, \
			.size = sizeof(int8_t), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_int8 = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(int8_t), \
		.deflt = _deflt, \
	}

/** Generate a single signed 8-bit property */
#define PROP_INT8(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_INT8_ARRAY(_name, _id, _flags, (const int8_t []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate a signed 16-bit array property */
#define PROP_INT16_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_INT16, \
			.size = sizeof(int16_t), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_int16 = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(int16_t), \
		.deflt = _deflt, \
	}

/** Generate a single signed 16-bit property */
#define PROP_INT16(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_INT16_ARRAY(_name, _id, _flags, (const int16_t []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate a signed 32-bit array property */
#define PROP_INT32_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_INT32, \
			.size = sizeof(int32_t), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_int32 = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(int32_t), \
		.deflt = _deflt, \
	}

/** Generate a single signed 32-bit property */
#define PROP_INT32(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_INT32_ARRAY(_name, _id, _flags, (const int32_t []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate a signed 64-bit array property */
#define PROP_INT64_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_INT64, \
			.size = sizeof(int64_t), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_int64 = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(int64_t), \
		.deflt = _deflt, \
	}

/** Generate a single signed 64-bit property */
#define PROP_INT64(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_INT64_ARRAY(_name, _id, _flags, (const int64_t []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate a single precision floating point array property */
#define PROP_FLOAT_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_FLOAT, \
			.size = sizeof(float), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_float = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(float), \
		.deflt = _deflt, \
	}

/** Generate a single single precision floating point property */
#define PROP_FLOAT(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_FLOAT_ARRAY(_name, _id, _flags, (const float []){_deflt}, 1, _min, _max, _fmt, _unit)

/** Generate a double precision floating point array property */
#define PROP_DOUBLE_ARRAY(_name, _id, _flags, _deflt, _count, _min, _max, _fmt, _unit) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_DOUBLE, \
			.size = sizeof(double), \
			.fmt = _fmt, \
			.unit = _unit, \
			.limits.limit_double = { \
				.min = _min, \
				.max = _max, \
			} \
		}, \
		.count = _count, \
		.size = _count * sizeof(double), \
		.deflt = _deflt, \
	}

/** Generate a single double precision floating point property */
#define PROP_DOUBLE(_name, _id, _flags, _deflt, _min, _max, _fmt, _unit) \
	PROP_DOUBLE_ARRAY(_name, _id, _flags, (const double []){_deflt}, 1, _min, _max, _fmt, _unit)


/** Generate a string array property */
#define PROP_STRING_ARRAY(_name, _id, _flags, _deflt, _count, _size) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_STRING, \
			.size = _size, \
		}, \
		.count = _count, \
		.size = _count * _size, \
		.deflt = _deflt, \
	}

/** Generate a single string property of size _size */
#define PROP_STRING(_name, _id, _flags, _deflt, _size) \
	PROP_STRING_ARRAY(_name, _id, _flags, _deflt, 1, _size) \

/** Generate a binary array property */
#define PROP_BINARY_ARRAY(_name, _id, _flags, _deflt, _count, _size) \
	{ \
		.name = _name, \
		.id = _id, \
		.flags = _flags, \
		.type = { \
			.type = PROP_TYPE_BINARY, \
			.size = _size, \
		}, \
		.count = _count, \
		.size = _count * _size, \
		.deflt = _deflt, \
	}

/** Generate a single binary property of size _size */
#define PROP_BINARY(_name, _id, _flags, _deflt, _size) \
	PROP_BINARY_ARRAY(_name, _id, _flags, _deflt, 1, _size) \

/** Generate a property group */
#define PROP_GROUP(_name, _id, _props, _desc) \
	{ \
		.name = _name, \
		.id = _id, \
		.desc = _desc, \
		.count = sizeof(_props)/sizeof(_props[0]), \
		.props = _props, \
	}

/** Generate a property subsystem */
#define PROP_SUBSYSTEM(_name, _desc, _groups) \
	{ \
		.name = _name, \
		.desc = _desc, \
		.count = sizeof(_groups)/sizeof(_groups[0]), \
		.groups = _groups, \
	}

/** Property types identifiers */
typedef enum {
	/* 0 reserved */
	PROP_TYPE_BOOL =	1,  /**< Boolean value */
	PROP_TYPE_UINT8 =	2,  /**< Unsigned 8-bit value */
	PROP_TYPE_UINT16 =	3,  /**< Unsigned 16-bit value */
	PROP_TYPE_UINT32 =	4,  /**< Unsigned 32-bit value */
	PROP_TYPE_UINT64 =	5,  /**< Unsigned 64-bit value */
	PROP_TYPE_INT8 =	6,  /**< Signed 8-bit value */
	PROP_TYPE_INT16 =	7,  /**< Signed 16-bit value */
	PROP_TYPE_INT32 =	8,  /**< Signed 32-bit value */
	PROP_TYPE_INT64 =	9,  /**< Signed 64-bit value */
	PROP_TYPE_DOUBLE =	10, /**< Double precision floating point value */
	PROP_TYPE_FLOAT =	11, /**< Single precision floating point value */
	PROP_TYPE_STRING =	12, /**< ASCII String value */
	PROP_TYPE_BINARY =	13, /**< Binary value */
	PROP_TYPE_ARRAY =	14, /**< Array value */
	/* 15 reserved */
} prop_type_t;

/** Unsigned 8-bit limits */
struct prop_limit_uint8 {
	uint8_t min;
	uint8_t max;
};

/** Unsigned 16-bit limits */
struct prop_limit_uint16 {
	uint16_t min;
	uint16_t max;
};

/** Unsigned 32-bit limits */
struct prop_limit_uint32 {
	uint32_t min;
	uint32_t max;
};

/** Unsigned 64-bit limits */
struct prop_limit_uint64 {
	uint64_t min;
	uint64_t max;
};

/** Signed 8-bit limits */
struct prop_limit_int8 {
	int8_t min;
	int8_t max;
};

/** Signed 16-bit limits */
struct prop_limit_int16 {
	int16_t min;
	int16_t max;
};

/** Signed 32-bit limits */
struct prop_limit_int32 {
	int32_t min;
	int32_t max;
};

/** Signed 64-bit limits */
struct prop_limit_int64 {
	int64_t min;
	int64_t max;
};

/** Single precision floating point limits */
struct prop_limit_float {
	float min;
	float max;
};

/** Double precision floating point limits */
struct prop_limit_double {
	double min;
	double max;
};

/** Property type */
struct prop_type {
	/** Type identifier */
	prop_type_t type;
	/** Size in bytes of 1 element of this type */
	size_t size;
	/** Format string override */
	const char *fmt;
	/** Unit */
	const char *unit;
	/** Value limits */
	union {
		struct prop_limit_uint8 limit_uint8;
		struct prop_limit_uint16 limit_uint16;
		struct prop_limit_uint32 limit_uint32;
		struct prop_limit_uint64 limit_uint64;
		struct prop_limit_int8 limit_int8;
		struct prop_limit_int16 limit_int16;
		struct prop_limit_int32 limit_int32;
		struct prop_limit_int64 limit_int64;
		struct prop_limit_float limit_float;
		struct prop_limit_double limit_double;
	} limits;
};

/** Property flags */
typedef enum {
	PROP_FLAG_NONE     = 0, /**< No property flags */
	PROP_FLAG_READONLY = (1 << 0), /**< Property can not be changed by user */
	PROP_FLAG_NOSTORE  = (1 << 1), /**< Property should not be saved/loaded */
	PROP_FLAG_SECRET   = (1 << 2), /**< Property value should not be listed */
} prop_flag_t;

/** Property specification */
struct prop_spec {
	/** Name of property */
	const char *name;
	/** ID of property */
	prop_id_t id;
	/** Flags */
	prop_flag_t flags;
	/** Property type specifier */
	struct prop_type type;
	/** Number of array elements */
	size_t count;
	/** Total size of value location */
	size_t size;
	/** Pointer to default value location */
	const void *deflt;
};

/** Property group specification */
struct prop_group_spec {
	/** Name of property group */
	const char *name;
	/** ID of property group */
	prop_id_t id;
	/** Description of group */
	const char *desc;
	/** Number of properties in props list */
	size_t count;
	/** List of properties in this group */
	const struct prop_spec *props;
};

/** Property subsystem specification */
struct prop_subsys_spec {
	/** Name of subsystem */
	const char *name;
	/** Description of subsystem */
	const char *desc;
	/** Number of properties in props list */
	size_t count;
	/** List of property groups in this subsystem */
	const struct prop_group_spec *groups;
};

#endif /* _SL_PROP_SPEC_TYPES_H_ */
