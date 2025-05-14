/*
 * Copyright (c) 2013-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_BTP_CRC32_H_
#define _SL_BTP_CRC32_H_

#include <stdint.h>
#include <unistd.h>
#include <hcc/api_fat.h>

int btp_crc32c_fd(F_FILE* fh, uint32_t *result, size_t size);

#endif /* _BTP_CRC32_H_ */
