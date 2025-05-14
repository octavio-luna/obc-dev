/*
 * Copyright (c) 2013-2023 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_BTP_BOUNDS_H_
#define _SL_BTP_BOUNDS_H_

#include <stdint.h>

#include <btp/client.h>

/**
 * @brief Update BTP bounds
 *
 * This functions updates next and bits based on the received BTP block.
 *
 * @param block_status BTP block status struct
 * @param block Received BTP block
 */
void btp_update_bounds(struct block_status *block_status, uint32_t block);

#endif /* _SL_BTP_BOUNDS_H_ */
