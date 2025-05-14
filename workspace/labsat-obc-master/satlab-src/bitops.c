/*
 * Copyright (c) 2013-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <btp/bitops.h>

#include <stdint.h>

uint8_t get_bit(uint8_t *buf, uint32_t bitnum)
{
	/* Return the bitnum'th bit in buf */
	return (buf[bitnum/8] >> (7 - bitnum % 8)) & 0x01;
}

void set_bit(uint8_t *buf, uint32_t bitnum)
{
	buf[bitnum/8] |= (1 << (7 - bitnum % 8));
}
