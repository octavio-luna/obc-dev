/*
 * Copyright (c) 2013-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_BTP_TYPES_H_
#define _SL_BTP_TYPES_H_

#include <stdint.h>

#define BACKEND_NAME_SIZE	5
#define BLOB_NAME_SIZE		38

#define BTP_BITFIELD_LENGTH	10

#define SHELL_INP_SIZE 		80
#define SHELL_OUTP_SIZE 	80

#define BTP_DEFAULT_PORT	13

enum btp_xfer_state {
	STATE_IDLE = 0,
	STATE_CONNECTED,
	STATE_UPLOAD,
	STATE_DOWNLOAD,
	STATE_LIST,
};

enum btp_type {
	/* Data */
	BTP_BLOCK_REQUEST = 0,	/**< Block request */
	BTP_BLOCK_REPLY,	/**< Block reply */
	/* Download */
	BTP_DW_REQUEST,		/**< Download blob request */
	BTP_DW_REPLY,		/**< Download blob reply */
	/* Upload */
	BTP_UP_REQUEST,		/**< Upload blob request */
	BTP_UP_REPLY,		/**< Upload blob reply */
	/* Pull Status */
	BTP_STAT_PULL_REQUEST,	/**< Blob pull status request */
	BTP_STAT_PULL_REPLY,	/**< Blob pull status reply */
	/* Push Status */
	BTP_STAT_PUSH_REQUEST,	/**< Blob push status request */
	BTP_STAT_PUSH_REPLY,	/**< Blob push status reply */
	/* Complete */
	BTP_COMPLETE_REQUEST,	/**< Completion request */
	BTP_COMPLETE_REPLY,	/**< Completion reply */
	/* List */
	BTP_LIST_REQUEST,	/**< List blobs request */
	BTP_LIST_REPLY,		/**< List blobs reply */
	/* Entry */
	BTP_ENTRY_REQUEST,	/**< Entry request */
	BTP_ENTRY_REPLY,	/**< Entry reply */
	/* Remove */
	BTP_REMOVE_REQUEST,	/**< Remove blob request */
	BTP_REMOVE_REPLY,	/**< Remove blob reply */
	/* Move */
	BTP_MOVE_REQUEST,	/**< Move blob request */
	BTP_MOVE_REPLY,		/**< Move blob reply */
	/* Copy */
	BTP_COPY_REQUEST,	/**< Copy blob request */
	BTP_COPY_REPLY,		/**< Copy blob reply */
	/* XXX UNSUPPORTED but reserved for backwards compatibility */
	__BTP_INFO_REQUEST,
	__BTP_INFO_REPLY,
	__BTP_LOCK_REQUEST,
	__BTP_LOCK_REPLY,
	__BTP_UNLOCK_REQUEST,
	__BTP_UNLOCK_REPLY,
	__BTP_GETATTRIB_REQUEST,
	__BTP_GETATTRIB_REPLY,
	__BTP_SETATTRIB_REQUEST,
	__BTP_SETATTRIB_REPLY,
	/* Shell */
	BTP_SHELL_REQUEST,	/**< Shell request */
	BTP_SHELL_REPLY,	/**< Shell reply */
	/* Mkdir */
	BTP_MKDIR_REQUEST,      /**< Mkdir blob request */
	BTP_MKDIR_REPLY,        /**< Mkdir blob reply */
	/* Rmdir */
	BTP_RMDIR_REQUEST,      /**< Rmdir blob request */
	BTP_RMDIR_REPLY,        /**< Rmdir blob reply */
} __attribute__ ((packed));

enum btp_completion {
	BTP_CPLT_OK,		/**< File received, close connection */
	BTP_CPLT_CRC32,		/**< Reply with CRC32, keep connection open */
	BTP_CPLT_DISCON,	/**< Disconnect and keep map */
	BTP_CPLT_ABORT,		/**< Remove file and map */
} __attribute__ ((packed));

/** Generic packet type */
struct btp_packet {
	uint8_t type;
	uint8_t err;
} __attribute__ ((packed));

/** Block pull request */
struct btp_blockreq {
	uint8_t type;		/**< Must be BTP_BLOCK_REQUEST */
	uint32_t offset;
	uint32_t count;
} __attribute__ ((packed));

/** Block pull reply */
struct btp_blockrep {
	uint8_t type;		/**< Must be BTP_BLOCK_REPLY */
	uint8_t err;
	uint32_t seq;
	uint32_t total;
	uint32_t block;
	uint8_t data[0];
} __attribute__ ((packed));

/** Download request */
struct btp_dwreq {
	uint8_t type;		/**< Must be BTP_DW_REQUEST */
	char backend[BACKEND_NAME_SIZE];
	char name[BLOB_NAME_SIZE];
	uint8_t block_size;
	uint32_t deprecated1;	/**< Deprecated, set to 0 */
	uint32_t deprecated2; 	/**< Deprecated, set to 0 */
	uint32_t timeout;
} __attribute__ ((packed));

/** Download reply */
struct btp_dwrep {
	uint8_t type;		/**< Must be BTP_DW_REPLY */
	uint8_t err;
	uint32_t checksum;
	uint32_t size;
} __attribute__ ((packed));

/** Upload request */
struct btp_upreq {
	uint8_t type;		/**< Must be BTP_UP_REQUEST */
	char backend[BACKEND_NAME_SIZE];
	char name[BLOB_NAME_SIZE];
	uint8_t block_size;
	uint32_t checksum;
	uint32_t size;
	uint32_t timeout;
	uint8_t force;
	uint8_t paranoid;
} __attribute__ ((packed));

/** Upload reply */
struct btp_uprep {
	uint8_t type;		/**< Must be BTP_UP_REPLY */
	uint8_t err;
	uint32_t checksum;
	uint32_t size;
} __attribute__ ((packed));

/** Status pull request */
struct btp_stat_pullrequest {
	uint8_t type;		/**< Must be BTP_STAT_PULL_REQUEST */
	uint32_t next;
	uint32_t bits;
	uint8_t bitfield[BTP_BITFIELD_LENGTH];
} __attribute__ ((packed));

/** Status pull reply */
struct btp_stat_pullreply {
	uint8_t type;		/**< Must be BTP_STAT_PULL_REPLY */
	uint8_t err;
} __attribute__ ((packed));

/** Status push request */
struct btp_stat_pushrequest {
	uint8_t type;		/**< Must be BTP_STAT_PUSH_REQUEST */
} __attribute__ ((packed));

/** Status push reply */
struct btp_stat_pushreply {
	uint8_t type;		/**< Must be BTP_STAT_PUSH_REPLY */
	uint8_t err;
	uint32_t next;
	uint32_t bits;
	uint8_t bitfield[BTP_BITFIELD_LENGTH];
} __attribute__ ((packed));

/** Complete request */
struct btp_completerequest {
	uint8_t type;		/**< Must be BTP_COMPLETE_REQUEST */
} __attribute__ ((packed));

/** Complete reply */
struct btp_completereply {
	uint8_t type;		/**< Must be BTP_COMPLETE_REPLY */
	uint8_t err;
} __attribute__ ((packed));

/** List request */
struct btp_listreq {
	uint8_t type;		/**< Must be BTP_LIST_REQUEST */
	uint16_t count;
	char backend[BACKEND_NAME_SIZE];
	char name[BLOB_NAME_SIZE];
} __attribute__ ((packed));

/** List reply */
struct btp_listrep {
	uint8_t type;		/**< Must be BTP_LIST_REPLY */
	uint8_t err;
	uint8_t entries;
} __attribute__ ((packed));

/** Entry request */
struct btp_entryreq {
	uint8_t type;		/**< Must be BTP_ENTRY_REQUEST */
	uint16_t count;
} __attribute__ ((packed));

/** Entry reply */
struct btp_entryrep {
	uint8_t type;		/**< Must be BTP_ENTRY_REPLY */
	uint8_t err;
	uint8_t seq;
	char name[BLOB_NAME_SIZE];
	uint32_t size;
	uint32_t mode;
	uint32_t etype;
	uint16_t uid;
	uint16_t gid;
	uint16_t nlink;
	uint32_t mtime;
} __attribute__ ((packed));

/** Remove request */
struct btp_removereq {
	uint8_t type;		/**< Must be BTP_REMOVE_REQUEST */
	char backend[BACKEND_NAME_SIZE];
	char name[BLOB_NAME_SIZE];
} __attribute__ ((packed));

/** Remove reply */
struct btp_removerep {
	uint8_t type;		/**< Must be BTP_REMOVE_REPLY */
	uint8_t err;
} __attribute__ ((packed));

/** Move request */
struct btp_movereq {
	uint8_t type;		/**< Must be BTP_MOVE_REQUEST */
	char backend[BACKEND_NAME_SIZE];
	char from[BLOB_NAME_SIZE];
	char to[BLOB_NAME_SIZE];
} __attribute__ ((packed));

/** Move reply */
struct btp_moverep {
	uint8_t type;		/**< Must be BTP_MOVE_REPLY */
	uint8_t err;
} __attribute__ ((packed));

/** Copy request */
struct btp_copyreq {
	uint8_t type;		/**< Must be BTP_COPY_REQUEST */
	char backend[BACKEND_NAME_SIZE];
	char from[BLOB_NAME_SIZE];
	char to[BLOB_NAME_SIZE];
} __attribute__ ((packed));

/** Copy reply */
struct btp_copyrep {
	uint8_t type;		/**< Must be BTP_COPY_REPLY */
	uint8_t err;
} __attribute__ ((packed));

#define SHELL_FLAG_CHAR	0x01
#define SHELL_FLAG_DONE	0x02

/** Shell request */
struct btp_shellreq {
	uint8_t type;		/**< Must be BTP_SHELL_REQUEST */
	uint8_t flags;
	uint16_t count;
	char input[SHELL_INP_SIZE];
} __attribute__ ((packed));

#define shell_request_size(_chars) (sizeof(struct btp_shellreq) - sizeof(((struct btp_shellreq *)0)->input) + _chars)

/** Shell reply */
struct btp_shellrep {
	uint8_t type;		/**< Must be BTP_SHELL_REPLY */
	uint8_t err;
	uint8_t seq;
	uint8_t flags;
	char output[SHELL_OUTP_SIZE];
} __attribute__ ((packed));

#define shell_reply_size(_chars) (sizeof(struct btp_shellrep) - sizeof(((struct btp_shellrep *)0)->output) + _chars)

/** Mkdir request */
struct btp_mkdirreq {
	uint8_t type;		/**< Must be BTP_MKDIR_REQUEST */
	char backend[BACKEND_NAME_SIZE];
	char name[BLOB_NAME_SIZE];
} __attribute__ ((packed));

/** Mkdir reply */
struct btp_mkdirrep {
	uint8_t type;		/**< Must be BTP_MKDIR_REPLY */
	uint8_t err;
} __attribute__ ((packed));

/** Rmdir request */
struct btp_rmdirreq {
	uint8_t type;		/**< Must be BTP_RMDIR_REQUEST */
	char backend[BACKEND_NAME_SIZE];
	char name[BLOB_NAME_SIZE];
} __attribute__ ((packed));

/** Rmdir reply */
struct btp_rmdirrep {
	uint8_t type;		/**< Must be BTP_RMDIR_REPLY */
	uint8_t err;
} __attribute__ ((packed));

#endif /* _BTP_TYPES_H_ */
