/*
 * Copyright (c) 2016-2023 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_CRC16CCITT_H_
#define _SL_CRC16CCITT_H_

#include <stdlib.h>
#include <stdint.h>

uint16_t crc16_ccitt_update(uint16_t crc, const uint8_t *input, size_t bytes);

uint16_t crc16_ccitt(const uint8_t *input, size_t bytes);

#endif /* _SL_CRC16CCITT_H_ */
