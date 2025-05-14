/*
 * Copyright (c) 2013-2023 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <btp/bitops.h>
#include <btp/client.h>
#include <btp/types.h>

static void shift_bits(uint8_t *bitfield, size_t length)
{
	for (int i = 0; i < length - 1; i++) {
		/* Shift byte 1 to the left and set LSB to MSB of previous byte */
		bitfield[i] = (bitfield[i] << 1) | (bitfield[i + 1] >> 7);
	}
	/* Least significant byte should just be shifted by 1 */
	bitfield[length - 1] = bitfield[length - 1] << 1;
}

void btp_update_bounds(struct block_status *block_status, uint32_t block)
{
	/*
	 * The blocks are marked as received in a bitfield.
	 * - 'next' marks the next packet we expect to receive.
	 * - 'bits' marks the block following the largest block
	 *   we have ever seen.
	 *
	 * [ |X|X|X| | |X| | | | | | | | ]
	 *  |	          |	        |
	 *  '-> next      '-> bits      '-> next + BTP_BITFIELD_LENGTH
	 *
	 * Where 'X' marks a received block. Both 'next' and 'bits'
	 * are initialized to 0.
	 *
	 * The difference between between 'bits' and 'next' can not
	 * be larger than the number of bits in btp_statusrep.bitfield,
	 * hence the somewhat complex update bounds function.
	 */

	/* Throw away blocks we already have or blocks we can not keep track of */
	if (block < block_status->next ||
	    block >= block_status->next + sizeof(block_status->block_status) * BITS_PER_BYTE ||
	    block >= block_status->blocks)
		return;

	/* Update bits */
	if (block >= block_status->bits)
		block_status->bits = block + 1;

	/* Mark this block received */
	set_bit(block_status->block_status, block - block_status->next);

	/* Update next expected block */
	while (get_bit(block_status->block_status, 0)) {
		block_status->next++;
		shift_bits(block_status->block_status, BTP_BITFIELD_LENGTH);
	}

	if (block_status->next == block_status->blocks)
		block_status->complete = true;
}
