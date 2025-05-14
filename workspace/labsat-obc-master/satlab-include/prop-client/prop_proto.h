/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file
 *
 * This file specifies the remote property access protocol. It should not be
 * necessary for clients to use these definitions directly. Instead, the
 * prop_client/prop_query functions should be used which handles the protocol
 * and performs necessary byte order conversion.
 *
 * The remote system listens for messages on a single CSP port (by default port
 * 19). The server only replies to request messages from clients, and does not
 * transmit messages on its own.
 *
 * Each message begins with an 8-bit type field. Unknown or non-request type
 * messages are silently dropped by the property server. All request frames
 * contain a flag byte. Currently, only a single flag is implemented, by
 * additional flags may be added in future revisions. Messages with unknown
 * flags are considered a protocol error and are silently dropped by the
 * server. All reply frames contain an 8-bit error code immediately after the
 * type field.
 *
 * Protocol fields are all transmitted in big-endian ("network") byte order.
 *
 * Some protocol messages are protected with a "key" field which must be set to
 * a predefined value before the message is accepted.
 *
 * Get/set request contain packed fields. For get requests, multiple property
 * IDs (or group IDs) can be requested by packing the 16-bit IDs in the data
 * buffer of the get request frame. For string type properties, the ID must be
 * followed by a 16-bit value with the size of the string. The remote system
 * replies with a packed buffer of property IDs and property values.
 *
 * For set requests, the data field contains packed property IDs, followed by
 * the new value of the corresponding property. Again, string type property IDs
 * must be followed by a 16-bit field with the length of the string argument.
 *
 * Both the request and reply can be split over multiple frames. The first
 * message in such a transaction must have the SL_PROP_FLAG_BEGIN flag set.
 */

#ifndef _SL_PROP_PROTO_H_
#define _SL_PROP_PROTO_H_

#include <stdint.h>

/** Default property service port */
#define SL_PROP_DEFAULT_PORT		19

/** Error codes */
#define SL_PROP_ERR_NONE		0 /**< No error */
#define SL_PROP_ERR_INVAL		1 /**< Invalid argument */
#define SL_PROP_ERR_NOENT		2 /**< No such file or directory */
#define SL_PROP_ERR_NOSPC		3 /**< No space left on device */
#define SL_PROP_ERR_IO			4 /**< I/O error */
#define SL_PROP_ERR_TIMEDOUT		5 /**< Timeout while waiting for data */

/** Message types */
#define SL_PROP_GET_REQUEST		1 /**< Property get request */
#define SL_PROP_GET_REPLY		2 /**< Property get reply */
#define SL_PROP_SET_REQUEST		11 /**< Property set request */
#define SL_PROP_SET_REPLY		12 /**< Property set reply */
#define SL_PROP_RESET_REQUEST		21 /**< Property reset request */
#define SL_PROP_RESET_REPLY		22 /**< Property reset reply */
#define SL_PROP_LOAD_REQUEST		31 /**< Property load request */
#define SL_PROP_LOAD_REPLY		32 /**< Property load reply */
#define SL_PROP_SAVE_REQUEST		41 /**< Property save request */
#define SL_PROP_SAVE_REPLY		42 /**< Property save reply */
#define SL_PROP_ERASE_REQUEST		51 /**< Property erase request */
#define SL_PROP_ERASE_REPLY		52 /**< Property erase reply */
#define SL_PROP_LOCK_REQUEST		61 /**< Property lock request */
#define SL_PROP_LOCK_REPLY		62 /**< Property lock reply */
#define SL_PROP_UNLOCK_REQUEST		71 /**< Property unlock request */
#define SL_PROP_UNLOCK_REPLY		72 /**< Property unlock reply */

/** Flag bits */
#define SL_PROP_FLAG_BEGIN		(1 << 0) /**< Set on first message in set/get request */

/** Known flag bits. Messages with unknown flags are dropped */
#define SL_PROP_FLAGS_KNOWN		(SL_PROP_FLAG_BEGIN)

/** Randomly generated magic keys */
#define SL_PROP_LOAD_BOOT_KEY		0xb5bd72e6 /** Property load boot key */
#define SL_PROP_LOAD_FALLBACK_KEY	0xc3493956 /** Property load fallback key */ 
#define SL_PROP_SAVE_BOOT_KEY		0xe1ab7c07 /** Property save boot key */ 
#define SL_PROP_SAVE_FALLBACK_KEY	0xf3c7cb7e /** Property save fallback key */ 
#define SL_PROP_ERASE_BOOT_KEY		0xe51792f0 /** Property erase boot key */ 
#define SL_PROP_ERASE_FALLBACK_KEY	0x323aad34 /** Property erase fallback key */ 
#define SL_PROP_LOCK_BOOT_KEY		0x6dcedbdb /** Property lock boot key */ 
#define SL_PROP_LOCK_FALLBACK_KEY	0x65e247e4 /** Property lock fallback key */ 
#define SL_PROP_UNLOCK_BOOT_KEY		0x41321b8f /** Property unlock boot key */ 
#define SL_PROP_UNLOCK_FALLBACK_KEY	0x0c71cae8 /** Property unlock fallback key */ 

/** Property get request */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_GET_REQUEST */
	uint8_t flags;		/**< Request flags. SL_PROP_FLAG_BEGIN must be set on first frame */
	uint8_t chunksize;	/**< Max number of bytes to return per reply frame. Ignored if SL_PROP_FLAG_BEGIN is not set */
	uint8_t remain;		/**< Remaining frames after this one */
	uint8_t data[0];	/**< Packed request data */
} __attribute__ ((packed)) sl_prop_get_req_t;

/** Property get reply */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_GET_REPLY */
	uint8_t error;		/**< Error code */
	uint8_t remain;		/**< Remaining frames after this one */
	uint8_t data[0];	/**< Packed reply data */
} __attribute__ ((packed)) sl_prop_get_rep_t;

/** Property set request */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_SET_REQUEST */
	uint8_t flags;		/**< Request flags. SL_PROP_FLAG_BEGIN must be set on first frame */
	uint8_t remain;		/**< Remaining frames after this one */
	uint8_t data[0];	/**< Packed request data */
} __attribute__ ((packed)) sl_prop_set_req_t;

/** Property set reply */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_SET_REPLY */
	uint8_t error;		/**< Error code */
} __attribute__ ((packed)) sl_prop_set_rep_t;

/** Property reset request */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_RESET_REQUEST */
	uint8_t flags;		/**< Request flags */
	uint8_t remain;		/**< Remaining frames after this one */
	uint16_t id;		/**< Group ID */
} __attribute__ ((packed)) sl_prop_reset_req_t;

/** Property reset reply */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_RESET_REPLY */
	uint8_t error;		/**< Error code */
} __attribute__ ((packed)) sl_prop_reset_rep_t;

/** Property load request */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_LOAD_REQUEST */
	uint8_t flags;		/**< Request flags */
	uint16_t id;		/**< Group ID */
	uint32_t key;		/**< SL_PROP_LOAD_BOOT_KEY/SL_PROP_LOAD_FALLBACK_KEY */
} __attribute__ ((packed)) sl_prop_load_req_t;

/** Property load reply */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_LOAD_REPLY */
	uint8_t error;		/**< Error code */
} __attribute__ ((packed)) sl_prop_load_rep_t;

/** Property save request */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_SAVE_REQUEST */
	uint8_t flags;		/**< Request flags */
	uint16_t id;		/**< Group ID */
	uint32_t key;		/**< SL_PROP_SAVE_BOOT_KEY/SL_PROP_SAVE_FALLBACK_KEY */
} __attribute__ ((packed)) sl_prop_save_req_t;

/** Property save reply */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_SAVE_REPLY */
	uint8_t error;		/**< Error code */
} __attribute__ ((packed)) sl_prop_save_rep_t;

/** Property erase request */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_ERASE_REQUEST */
	uint8_t flags;		/**< Request flags */
	uint16_t id;		/**< Group ID */
	uint32_t key;		/**< SL_PROP_ERASE_BOOT_KEY/SL_PROP_ERASE_FALLBACK_KEY */
} __attribute__ ((packed)) sl_prop_erase_req_t;

/** Property erase reply */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_ERASE_REPLY */
	uint8_t error;		/**< Error code */
} __attribute__ ((packed)) sl_prop_erase_rep_t;

/** Property lock request */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_LOCK_REQUEST */
	uint8_t flags;		/**< Request flags */
	uint16_t id;		/**< Group ID */
	uint32_t key;		/**< SL_PROP_LOCK_KEY */
} __attribute__ ((packed)) sl_prop_lock_req_t;

/** Property lock reply */
typedef struct {
	uint8_t type;		/**< Type must be SL_PROP_LOCK_REPLY */
	uint8_t error;		/**< Error code */
} __attribute__ ((packed)) sl_prop_lock_rep_t;

/** Property unlock request */
typedef struct {
	uint8_t type;		/* Type must be SL_PROP_UNLOCK_REQUEST */
	uint8_t flags;		/* Request flags */
	uint16_t id;		/* Group ID */
	uint32_t key;		/* SL_PROP_UNLOCK_KEY */
} __attribute__ ((packed)) sl_prop_unlock_req_t;

/** Property unlock reply */
typedef struct {
	uint8_t type;		/* Type must be SL_PROP_UNLOCK_REPLY */
	uint8_t error;		/* Error code */
} __attribute__ ((packed)) sl_prop_unlock_rep_t;

#endif /* _SL_PROP_PROTO_H_ */
