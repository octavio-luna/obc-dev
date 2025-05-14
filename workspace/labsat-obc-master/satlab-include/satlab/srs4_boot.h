/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

/**
 * @file srs4_boot.h
 *
 * This file contains functions to configure the boot process on a SRS-4
 * system.
 *
 * The system is delivered with a factory firmware flashed in the first half of
 * internal flash. An alternate image can be flashed on orbit in the "alternate
 * partion" in the second half of internal flash. An alternate firmware image
 * (ending in "-alt.bin") must be transferred to the NOR-flash using BTP and
 * can then be flashed to the alternate partition using the @ref
 * sl_srs4_bootloader_flash_alternate function.
 */

#ifndef _SL_SRS4_CLIENT_BOOT_H_
#define _SL_SRS4_CLIENT_BOOT_H_

#include <stdint.h>

/**
 * @brief Request boot of the alternate image
 *
 * The @p boots argument can be set 0 to boot the factory firmware. Note that a
 * system reset, e.g. using CSP reboot or external power cycle, is required
 * after this command to actually boot the image.
 *
 * After @p boots boot cycles, the system will return to the factory firmware.
 *
 * @param node CSP address of the property node.
 * @param timeout Timeout of the command in milliseconds.
 * @param boots Number of boots to boot the alternate image.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_srs4_bootloader_set_alternate(uint8_t node, uint32_t timeout, uint8_t boots);

/**
 * @brief Flash image file from NOR-flash to alternate partition
 *
 * This function flashes a firmware file from the NOR-flash to the alternate
 * flash partion. It is not necessary to erase the partition first.
 *
 * The system calculates the checksum of the file and verifies the contents of
 * the flash after programming. The calculated checksum is returned in the @p
 * crc argument and can be used to verify the value on ground.
 *
 * Flashing and verifying a firmware image typically takes 15-30 seconds, so
 * ensure that the timeout is set sufficiently high.
 *
 * It is not possible to flash an image when the alternate image is running. It
 * must be performed from the factory image.
 *
 * @warning The firmware file must be an "-alt" variant binary.
 *
 * @param node CSP address of the property node.
 * @param timeout Timeout of the command in milliseconds.
 * @param filename Filename of the remote file to flash.
 * @param crc Returned CRC32C checksum of flashed file.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_srs4_bootloader_flash_alternate(uint8_t node, uint32_t timeout, const char *filename, uint32_t *crc);

/**
 * @brief Erase alternate image partition
 *
 * Erase the contents of the alternate firmware partition.
 *
 * Erasing the alternate firmware partition typically takes 10-15 seconds, so
 * ensure that the timeout is set sufficiently high.
 *
 * It is not possible to erase the alternate image partition when the alternate
 * image is running. It must be performed from the factory image.
 *
 * @param node CSP address of the property node.
 * @param timeout Timeout of the command in milliseconds.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_srs4_bootloader_erase_alternate(uint8_t node, uint32_t timeout);

/**
 * @brief Calculate checksum of alternate partition contents
 *
 * This function calculates the CRC32C checksum of the contents of the
 * alternate partition, and returns the result in the @p crc argument. The
 * calculation always starts from the beginning of the alternate partition.
 *
 * @param node CSP address of the property node.
 * @param timeout Timeout of the command in milliseconds.
 * @param size Number of bytes to checksum.
 * @param crc Returned CRC32C checksum of flashed file.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_srs4_bootloader_checksum_alternate(uint8_t node, uint32_t timeout, uint32_t size, uint32_t *crc);

/**
 * @brief Verify partition image
 *
 * This function verifies the image in either the primary or alternate partion.
 *
 * @param node CSP address of the property node.
 * @param timeout Timeout of the command in milliseconds.
 * @param partition Partition number. 0 for primary, 1 for alternate.
 * @param size Read size of image.
 * @param checksum Calculated checksum of image.
 *
 * @returns 0 on success, negative error code otherwise.
 */
int sl_srs4_bootloader_verify(uint8_t node, uint32_t timeout, uint8_t partition, uint32_t *size, uint32_t *checksum);

#endif /* _SL_SRS4_CLIENT_BOOT_H_ */
