/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_SRS4_CLIENT_PROTO_H_
#define _SL_SRS4_CLIENT_PROTO_H_

#include <stdint.h>

/* Default CSP address */
#define SL_SRS4_DEFAULT_ADDRESS		24

/** CSP ports */
#define SL_SRS4_PORT_BOOTLOADER		25
#define SL_SRS4_PORT_SHELL		26

/** Error codes */
#define SL_SRS4_ERR_NONE		0
#define SL_SRS4_ERR_INVAL		1
#define SL_SRS4_ERR_IO			2
#define SL_SRS4_ERR_NOMEM		3
#define SL_SRS4_ERR_BUSY		4

/* Generic packet types */
typedef struct {
	uint8_t type;
} __attribute__ ((packed)) sl_srs4_req_t;

typedef struct {
	uint8_t type;
	uint8_t error;
} __attribute__ ((packed)) sl_srs4_rep_t;

/*
 * SL_SRS4_PORT_BOOTLOADER
 */

/* Types */
#define SL_SRS4_BOOTLOADER_SET_REQ		1
#define SL_SRS4_BOOTLOADER_SET_REP		2
#define SL_SRS4_BOOTLOADER_FLASH_REQ		5
#define SL_SRS4_BOOTLOADER_FLASH_REP		6
#define SL_SRS4_BOOTLOADER_ERASE_REQ		7
#define SL_SRS4_BOOTLOADER_ERASE_REP		8
#define SL_SRS4_BOOTLOADER_CHECKSUM_REQ		9
#define SL_SRS4_BOOTLOADER_CHECKSUM_REP		10
#define SL_SRS4_BOOTLOADER_VERIFY_REQ		11
#define SL_SRS4_BOOTLOADER_VERIFY_REP		12

/* Keys */
#define SL_SRS4_BOOTLOADER_SET_KEY		0x4f479511
#define SL_SRS4_BOOTLOADER_FLASH_KEY		0x1f57ef1f
#define SL_SRS4_BOOTLOADER_ERASE_KEY		0xcd6d7fb8

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_SET_REQ */
	uint8_t type;
	/* Flags, currently unused - set to zero */
	uint8_t flags;
	/* Number of boots to boot alternate image, 0 = boot factory */
	uint8_t boots;
	/* Must be SL_SRS4_BOOTLOADER_SET_KEY */
	uint32_t key;
} __attribute__ ((packed)) sl_srs4_bootloader_set_req_t;

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_SET_REP */
	uint8_t type;
	/* Error code */
	uint8_t error;
} __attribute__ ((packed)) sl_srs4_bootloader_set_rep_t;

#define SL_SRS4_BOOTLOADER_FILENAME_MAX	40
typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_FLASH_REQ */
	uint8_t type;
	/* Flags, currently unused - set to zero */
	uint8_t flags;
	/* Must be SL_SRS4_BOOTLOADER_FLASH_KEY */
	uint32_t key;
	/* Filename to flash to the alternate partition */
	uint8_t filename[SL_SRS4_BOOTLOADER_FILENAME_MAX];
} __attribute__ ((packed)) sl_srs4_bootloader_flash_req_t;

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_FLASH_REP */
	uint8_t type;
	/* Error code */
	uint8_t error;
	/* CRC32C of flash image */
	uint32_t checksum;
} __attribute__ ((packed)) sl_srs4_bootloader_flash_rep_t;

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_ERASE_REQ */
	uint8_t type;
	/* Flags, currently unused - set to zero */
	uint8_t flags;
	/* Must be SL_SRS4_BOOTLOADER_ERASE_KEY */
	uint32_t key;
} __attribute__ ((packed)) sl_srs4_bootloader_erase_req_t;

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_ERASE_REP */
	uint8_t type;
	/* Error code */
	uint8_t error;
} __attribute__ ((packed)) sl_srs4_bootloader_erase_rep_t;

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_CHECKSUM_REQ */
	uint8_t type;
	/* Flags, currently unused - set to zero */
	uint8_t flags;
	/* Number of bytes to checksum from beginning of alternate partition */
	uint32_t size;
} __attribute__ ((packed)) sl_srs4_bootloader_checksum_req_t;

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_CHECKSUM_REP */
	uint8_t type;
	/* Error code */
	uint8_t error;
	/* CRC32C of flash image */
	uint32_t checksum;
} __attribute__ ((packed)) sl_srs4_bootloader_checksum_rep_t;

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_VERIFY_REQ */
	uint8_t type;
	/* Flags, currently unused - set to zero */
	uint8_t flags;
	/* Partition to verify (0 = primary, 1 = alternate) */
	uint8_t partition;
} __attribute__ ((packed)) sl_srs4_bootloader_verify_req_t;

typedef struct {
	/* Must be SL_SRS4_BOOTLOADER_VERIFY_REP */
	uint8_t type;
	/* Error code */
	uint8_t error;
	/* Read size of image */
	uint32_t size;
	/* Calculated checksum of image */
	uint32_t checksum;
} __attribute__ ((packed)) sl_srs4_bootloader_verify_rep_t;

/*
 * SL_SRS4_PORT_SHELL
 */

/* Types */
#define SL_SRS4_SHELL_RUN_REQ		1
#define SL_SRS4_SHELL_RUN_REP		2

/* Keys */
#define SL_SRS4_SHELL_RUN_KEY		0xd9bda0d2

#define SL_SRS4_SHELL_RUN_MAX		100
typedef struct {
	/** Must be SL_SRS4_SHELL_RUN_REQ */
	uint8_t type;
	/** Flags, currently unused - set to zero */
	uint8_t flags;
	/** Must be SL_SRS4_SHELL_RUN_KEY */
	uint32_t key;
	/** Command to execute */
	uint8_t cmd[SL_SRS4_SHELL_RUN_MAX];
} __attribute__ ((packed)) sl_srs4_shell_run_req_t;

typedef struct {
	/** Must be SL_SRS4_SHELL_RUN_REP */
	uint8_t type;
	/** Error code */
	uint8_t error;
} __attribute__ ((packed)) sl_srs4_shell_run_rep_t;

#endif /* _SL_SRS4_CLIENT_PROTO_H_ */
