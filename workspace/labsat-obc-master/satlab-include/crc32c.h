/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_CRC32C_H_
#define _SL_CRC32C_H_

#include <stdlib.h>
#include <stdint.h>

uint32_t crc32c_update(uint32_t crc, const uint8_t *input, size_t bytes);

uint32_t crc32c(const uint8_t *input, size_t bytes);

#endif /* _SL_CRC32C_H_ */
