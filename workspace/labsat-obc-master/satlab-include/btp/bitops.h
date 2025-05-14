/*
 * Copyright (c) 2013-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_BTP_BITOPS_H_
#define _SL_BTP_BITOPS_H_

#include <stdint.h>

uint8_t get_bit(uint8_t *buf, uint32_t bitnum);

void set_bit(uint8_t *buf, uint32_t bitnum);

#endif /* _BTP_BITOPS_H_ */
