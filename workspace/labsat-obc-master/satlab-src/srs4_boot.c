/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <satlab/srs4_boot.h>

#include <errno.h>

#include <csp/csp.h>
#include <endian.h>

#include <satlab/srs4.h>

int sl_srs4_bootloader_set_alternate(uint8_t node, uint32_t timeout, uint8_t boots)
{
	int ret;
	sl_srs4_bootloader_set_req_t request;
	sl_srs4_bootloader_set_rep_t reply;

	request.type = SL_SRS4_BOOTLOADER_SET_REQ;
	request.flags = 0;
	request.boots = boots;
	request.key = csp_hton32(SL_SRS4_BOOTLOADER_SET_KEY);

	ret = csp_transaction(CSP_PRIO_NORM, node, SL_SRS4_PORT_BOOTLOADER, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_SRS4_BOOTLOADER_SET_REP)
		return -EINVAL;

	if (reply.error != SL_SRS4_ERR_NONE)
		return -reply.error;

	return 0;
}

int sl_srs4_bootloader_flash_alternate(uint8_t node, uint32_t timeout, const char *filename, uint32_t *crc)
{
	int ret;
	sl_srs4_bootloader_flash_req_t request;
	sl_srs4_bootloader_flash_rep_t reply;

	request.type = SL_SRS4_BOOTLOADER_FLASH_REQ;
	request.flags = 0;
	request.key = csp_hton32(SL_SRS4_BOOTLOADER_FLASH_KEY);

	if (strlen(filename) >= sizeof(request.filename))
		return -ENOSPC;

	strncpy((char *)request.filename, filename, sizeof(request.filename) - 1);
	request.filename[sizeof(request.filename) - 1] = '\0';

	ret = csp_transaction(CSP_PRIO_NORM, node, SL_SRS4_PORT_BOOTLOADER, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_SRS4_BOOTLOADER_FLASH_REP)
		return -EINVAL;

	if (reply.error != SL_SRS4_ERR_NONE)
		return -reply.error;

	if (crc != NULL)
		*crc = csp_ntoh32(reply.checksum);

	return 0;
}

int sl_srs4_bootloader_erase_alternate(uint8_t node, uint32_t timeout)
{
	int ret;
	sl_srs4_bootloader_erase_req_t request;
	sl_srs4_bootloader_erase_rep_t reply;

	request.type = SL_SRS4_BOOTLOADER_ERASE_REQ;
	request.flags = 0;
	request.key = csp_hton32(SL_SRS4_BOOTLOADER_ERASE_KEY);

	ret = csp_transaction(CSP_PRIO_NORM, node, SL_SRS4_PORT_BOOTLOADER, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_SRS4_BOOTLOADER_ERASE_REP)
		return -EINVAL;

	if (reply.error != SL_SRS4_ERR_NONE)
		return -reply.error;

	return 0;
}

int sl_srs4_bootloader_checksum_alternate(uint8_t node, uint32_t timeout, uint32_t size, uint32_t *crc)
{
	int ret;
	sl_srs4_bootloader_checksum_req_t request;
	sl_srs4_bootloader_checksum_rep_t reply;

	request.type = SL_SRS4_BOOTLOADER_CHECKSUM_REQ;
	request.flags = 0;
	request.size = csp_hton32(size);

	ret = csp_transaction(CSP_PRIO_NORM, node, SL_SRS4_PORT_BOOTLOADER, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_SRS4_BOOTLOADER_CHECKSUM_REP)
		return -EINVAL;

	if (reply.error != SL_SRS4_ERR_NONE)
		return -reply.error;

	if (crc != NULL)
		*crc = csp_ntoh32(reply.checksum);

	return 0;
}

int sl_srs4_bootloader_verify(uint8_t node, uint32_t timeout, uint8_t partition, uint32_t *size, uint32_t *checksum)
{
	int ret;
	sl_srs4_bootloader_verify_req_t request;
	sl_srs4_bootloader_verify_rep_t reply;

	request.type = SL_SRS4_BOOTLOADER_VERIFY_REQ;
	request.flags = 0;
	request.partition = partition;

	ret = csp_transaction(CSP_PRIO_NORM, node, SL_SRS4_PORT_BOOTLOADER, timeout,
			      &request, sizeof(request), &reply, sizeof(reply));
	if (ret != sizeof(reply))
		return -ECONNREFUSED;

	if (reply.type != SL_SRS4_BOOTLOADER_VERIFY_REP)
		return -EINVAL;

	*size = csp_ntoh32(reply.size);
	*checksum = csp_ntoh32(reply.checksum);

	if (reply.error != SL_SRS4_ERR_NONE)
		return -reply.error;

	return 0;
}
