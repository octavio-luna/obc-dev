/*
 * Copyright (c) 2013-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <errno.h>

#include <btp/bitops.h>
#include <btp/bounds.h>
#include <btp/client.h>
#include <btp/types.h>
#include <btp/error.h>
#include <btp/crc32.h>

#include <csp/csp.h>
#include <endian.h> //* csp 2.0 eliminated csp_endian and uses system endian.h insdtead */

#define MAP_SYMBOL_OK	"+"
#define MAP_SYMBOL_MISS	"-"
static const char *map_symbol_ok = MAP_SYMBOL_OK;
static const char *map_symbol_miss = MAP_SYMBOL_MISS;

#include <SDManager.h>
#include <LogManager.h>
#include <freertos/FreeRTOS.h>

bool btp_client_finished(struct btp_context *btp)
{
	if (!btp || (btp->state != STATE_UPLOAD && btp->state != STATE_DOWNLOAD))
		return BTP_EINVAL;

	return btp->block_status.complete;
}

struct btp_context *btp_client_connect(uint8_t host,
				       uint8_t port,
				       uint32_t timeout,
				       uint32_t opts,
				       unsigned int attempts)
{
	struct btp_context *btp;

	btp = (struct btp_context*)pvPortMalloc(sizeof(*btp));
	if (!btp) return NULL;

	btp->conn = csp_connect(CSP_PRIO_NORM, host, port, timeout, opts);
	if (!btp->conn) {
		vPortFree(btp);
		return NULL;
	}

	btp->host = host;
	btp->port = port;
	btp->timeout = timeout;
	btp->attempts = attempts;
	btp->block_status.complete = false;
	btp->state = STATE_CONNECTED;
	btp->fh = 0;

	UPDEBUG("connected to %u:%u\n", host, port);

	return btp;
}

int btp_client_disconnect(struct btp_context *btp)
{
	if (!btp || btp->state == STATE_IDLE)
		return BTP_EINVAL;

	csp_close(btp->conn);
	vPortFree(btp);

	return BTP_EOK;
}

int btp_client_upload(struct btp_context *btp,
		      const char *backend,
		      const char *remotepath,
		      const char *localpath,
		      uint8_t block_size,
		      uint32_t timeout_csum,
		      uint32_t timeout_server,
		      bool force)
{
	int i, ret;
	struct btp_upreq req;
	struct btp_uprep rep;
	uint32_t checksum;

	if (!btp || btp->state != STATE_CONNECTED)
		return BTP_EINVAL;

	req.type = BTP_UP_REQUEST;
	strncpy(req.backend, backend, sizeof(req.backend) - 1);
	req.backend[sizeof(req.backend) - 1] = '\0';
	strncpy(req.name, remotepath, sizeof(req.name) - 1);
	req.name[sizeof(req.name) - 1] = '\0';
	req.block_size = block_size;
	req.timeout = htole32(timeout_server);
	req.force = force;

	btp->fh = f_open(localpath, "r");
	if(btp->fh==0) return BTP_ENOENT;

	f_seek(btp->fh, 0, SEEK_END);
	btp->size = f_tell(btp->fh);
	f_seek(btp->fh, 0, SEEK_SET);

	if (btp_crc32c_fd(btp->fh, &checksum, 0) < 0)
		return BTP_EIO;

	btp->checksum = checksum;
	btp->block_status.block_size = block_size;
	btp->block_status.blocks = (btp->size + btp->block_status.block_size - 1) /
	                               btp->block_status.block_size;
	memset(btp->block_status.block_status, 0, sizeof(btp->block_status.block_status));
	btp->block_status.next = 0;
	btp->block_status.bits = 0;

	req.checksum = htole32(btp->checksum);
	req.size = htole32(btp->size);

	UPDEBUG("upload file %s of size %u bytes with checksum %08x\n", localpath, btp->size, btp->checksum);

	for (i = 0; i < btp->attempts; i++) {
		ret = csp_transaction_persistent(btp->conn, timeout_csum, &req, sizeof(req), &rep, sizeof(rep));
		if (ret != 0)
			break;
	}

	if (ret == 0)
		return BTP_ETIMEDOUT;

	if (rep.type != BTP_UP_REPLY)
		return BTP_EPROTO;

	if (rep.err != BTP_EOK)
		return rep.err;

	btp->state = STATE_UPLOAD;

	if (le32toh(rep.size) == btp->size && le32toh(rep.checksum) == btp->checksum)
		btp->block_status.complete = true;

	return BTP_EOK;
}

int btp_client_download(struct btp_context *btp,
			const char *backend,
			const char *remotepath,
			const char *localpath,
			uint8_t block_size,
			uint32_t timeout_csum,
			uint32_t timeout_server,
			bool force)
{
	int i, ret;
	char c;
	struct btp_dwreq req;
	struct btp_dwrep rep;
	uint32_t cursize, checksum;

	if (!btp || btp->state != STATE_CONNECTED)
		return BTP_EINVAL;

	strncpy(btp->filename, localpath, sizeof(btp->filename) - 1);
	btp->filename[sizeof(btp->filename) - 1] = '\0';

	req.type = BTP_DW_REQUEST;
	strncpy(req.backend, backend, sizeof(req.backend) - 1);
	req.backend[sizeof(req.backend) - 1] = '\0';
	strncpy(req.name, remotepath, sizeof(req.name) - 1);
	req.name[sizeof(req.name) - 1] = '\0';
	req.block_size = block_size;
	req.timeout = htole32(timeout_server);

	/* Deprecated fields */
	req.deprecated1 = 0;
	req.deprecated2 = 0;

	UPDEBUG("download file %s\n", remotepath);

	for (i = 0; i < btp->attempts; i++) {
		ret = csp_transaction_persistent(btp->conn, timeout_csum, &req, sizeof(req), &rep, sizeof(rep));
		if (ret != 0)
			break;
	}

	if (ret == 0)
		return BTP_ETIMEDOUT;

	if (rep.type != BTP_DW_REPLY)
		return BTP_EPROTO;

	if (rep.err != BTP_EOK)
		return rep.err;

	btp->state = STATE_DOWNLOAD;
	btp->size = le32toh(rep.size);
	btp->checksum = le32toh(rep.checksum);
	btp->block_status.block_size = block_size;
	btp->block_status.blocks = (btp->size + btp->block_status.block_size - 1) /
	                               btp->block_status.block_size;
	memset(btp->block_status.block_status, 0, sizeof(btp->block_status.block_status));
	btp->block_status.next = 0;
	btp->block_status.bits = 0;

	if (force)
		unlink(btp->filename);

	/* Check if file was already downloaded */
	btp->fh = f_open(btp->filename, "r+");
	if (btp->fh > 0) {
		f_seek(btp->fh, 0, SEEK_END);
		cursize = f_tell(btp->fh);
		f_seek(btp->fh, 0, SEEK_SET);

		if (cursize != btp->size) {
			ret = BTP_EEXIST;
			goto close_conn;
		}

		if (btp_crc32c_fd(btp->fh, &checksum, 0) < 0) {
			ret = BTP_EIO;
			goto close_conn;
		}

		if (checksum != btp->checksum) {
			ret = BTP_EEXIST;
			goto close_conn;
		}

		btp->block_status.complete = true;
		return BTP_EOK;
	}

	/* Check for partial transfer */
	snprintf(btp->filename_partial,
		 sizeof(btp->filename_partial) - 1,
		 "%s.btp",
		 btp->filename);
	btp->filename_partial[sizeof(btp->filename_partial) - 1] = '\0';

	if (force)
		unlink(btp->filename_partial);

	btp->fh = f_open(btp->filename_partial, "r+");
	if (btp->fh > 0) {
		/* Partial transfer */
		f_seek(btp->fh, 0, SEEK_END);
		cursize = f_tell(btp->fh);
		f_seek(btp->fh, 0, SEEK_SET);

		if (cursize != btp->size + btp->block_status.blocks) {
			printf("%s has wrong size for this block size (expected %"PRIu32" bytes)\n",
			       btp->filename_partial, btp->size + btp->block_status.blocks);
			ret = BTP_EEXIST;
			goto close_conn;
		}

		f_seek(btp->fh, btp->size, SEEK_SET);
		for (i = 0; i < btp->block_status.blocks; i++) {
			if (f_read(&c, 1, 1, btp->fh) != 1 || (c != '+' && c != '-')) {
				UPLOG_ERR("%s has wrong format\n", btp->filename_partial);
				ret = BTP_EINVAL;
				goto close_conn;
			} else {
				if (c == '+')
					btp_update_bounds(&btp->block_status, i);
			}
		}
		f_seek(btp->fh, 0, SEEK_SET);

		if (btp_crc32c_fd(btp->fh, &checksum, btp->size) < 0) {
			ret = BTP_EIO;
			goto close_conn;
		}

		if (checksum == btp->checksum)
			btp->block_status.complete = true;

		return BTP_EOK;
	}

	/* New transfer */
	btp->fh = f_open(btp->filename_partial, "w+");
	if (btp->fh==0) {
		ret = BTP_EIO;
		goto close_conn;
	}

	f_seek(btp->fh, btp->size, SEEK_SET);
	for (i = 0; i < btp->block_status.blocks; i++) {
		if (f_write(map_symbol_miss, 1, 1, btp->fh) != 1) {
			ret = BTP_EIO;
			goto close_conn;
		}
	}
	f_seek(btp->fh, 0, SEEK_SET);

	return BTP_EOK;

close_conn:
	if (btp_client_complete(btp, timeout_csum) != 0)
		UPLOG_NOTICE("Failed to close connection to server\n");

	return ret;

}

int btp_client_request_status(struct btp_context *btp)
{
	int i, ret;
	struct btp_stat_pushrequest req;
	struct btp_stat_pushreply rep;

	if (!btp || btp->state != STATE_UPLOAD)
		return BTP_EINVAL;

	req.type = BTP_STAT_PUSH_REQUEST;

	for (i = 0; i < btp->attempts; i++) {
		ret = csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), &rep, sizeof(rep));
		if (ret != 0)
			break;
	}

	if (ret == 0)
		return BTP_ETIMEDOUT;

	if (rep.type != BTP_STAT_PUSH_REPLY)
		return BTP_EPROTO;

	if (rep.err != BTP_EOK)
		return rep.err;

	if (btp->block_status.next > 0)
		btp->progress += (le32toh(rep.next) - btp->block_status.next) * btp->block_status.block_size;

	btp->block_status.next = le32toh(rep.next);
	btp->block_status.bits = le32toh(rep.bits);

	if (btp->block_status.next > btp->block_status.blocks ||
	    btp->block_status.bits > btp->block_status.blocks ||
	    (btp->block_status.bits - btp->block_status.next) > (BTP_BITFIELD_LENGTH * BITS_PER_BYTE))
		return BTP_EPROTO;

	memcpy(btp->block_status.block_status, rep.bitfield, sizeof(rep.bitfield));

	int j;
	UPDEBUG("Received status: G=%"PRIu32" B=%"PRIu32" I=[", btp->block_status.next, btp->block_status.bits);
	for (j = 0; j < (BTP_BITFIELD_LENGTH * BITS_PER_BYTE); j++) {
		UPDEBUG("%c", get_bit(btp->block_status.block_status, j) ? 'X' : '.');
	}
	UPDEBUG("]\n");

	if (le32toh(rep.next) == btp->block_status.blocks)
		btp->block_status.complete = true;

	return BTP_EOK;
}

int btp_client_send_status(struct btp_context *btp)
{
	int i, ret;
	struct btp_stat_pullrequest req;
	struct btp_stat_pullreply rep;

	if (!btp || btp->state != STATE_DOWNLOAD)
		return BTP_EINVAL;

	req.type = BTP_STAT_PULL_REQUEST;
	req.next = htole32(btp->block_status.next);
	req.bits = htole32(btp->block_status.bits);
	memcpy(req.bitfield, btp->block_status.block_status, sizeof(req.bitfield));

	int j;
	UPDEBUG("Sending status: G=%"PRIu32" B=%"PRIu32" I=[", btp->block_status.next, btp->block_status.bits);
	for (j = 0; j < (BTP_BITFIELD_LENGTH * BITS_PER_BYTE); j++) {
		UPDEBUG("%c", get_bit(btp->block_status.block_status, j) ? 'X' : '.');
	}
	UPDEBUG("]\n");

	for (i = 0; i < btp->attempts; i++) {
		ret = csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), &rep, sizeof(rep));
		if (ret != 0)
			break;
	}

	if (ret == 0)
		return BTP_ETIMEDOUT;

	return rep.err;
}

int btp_client_get_blocks(struct btp_context *btp, unsigned int offset, unsigned int count, btp_progress_cb cb, void *cbarg)
{
	int i, j, timeout_count;
	uint32_t remain, bytes;
	csp_packet_t *packet;
	struct btp_blockreq req;
	struct btp_blockrep *rep;
	bool received_data = false;

	if (!btp || btp->state != STATE_DOWNLOAD)
		return BTP_EINVAL;

	req.type = BTP_BLOCK_REQUEST;
	req.offset = htole32(offset);
	req.count = htole32(count);

	for (i = 0; i < btp->attempts && !received_data; i++) {
		UPDEBUG("sending get blocks\n");
		if (csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), NULL, 0) == 0)
			continue;

		timeout_count = 0;

		for (j = 0; j < count; j++) {
			packet = csp_read(btp->conn, btp->timeout);

			if (!packet) {
				UPDEBUG("waiting for more packets\n");
				timeout_count++;
				if (timeout_count >= 5)
					break;
				continue;
			}

			received_data = true;

			rep = (struct btp_blockrep *) packet->data;

			if (rep->type != BTP_BLOCK_REPLY) {
				UPDEBUG("not block reply\n");
				csp_buffer_free(packet);
				return BTP_EPROTO;
			}

			if (rep->err != BTP_EOK) {
				UPDEBUG("not EOK\n");
				csp_buffer_free(packet);
				return rep->err;
			}

			if (le32toh(rep->block) < btp->block_status.next) {
				UPDEBUG("block (%"PRIu32") < next (%"PRIu32")\n", le32toh(rep->block), btp->block_status.next);
				csp_buffer_free(packet);
				continue;
			}

			if (f_seek(btp->fh, le32toh(rep->block) * btp->block_status.block_size, SEEK_SET)!=0) {
				UPLOG_ERR("f_seek failed\n");
				return BTP_EIO;
			}

			remain = btp->size - le32toh(rep->block) * btp->block_status.block_size;
			bytes = remain >= btp->block_status.block_size ? btp->block_status.block_size : remain;

			if (f_write(rep->data, 1, bytes, btp->fh) < bytes) {
				printf("Failed to write data\n");
				return BTP_EIO;
			}

			if (f_seek(btp->fh, btp->size + le32toh(rep->block), SEEK_SET)!=0) {
				UPLOG_ERR("f_seek map failed\n");
				return BTP_EIO;
			}

			if (f_write(map_symbol_ok, 1, 1, btp->fh) != 1) {
				printf("Failed to write map\n");
				return BTP_EIO;
			}

			btp->progress += bytes;

			UPDEBUG("received block %"PRIu32"\n", le32toh(rep->block));
			btp_update_bounds(&btp->block_status, le32toh(rep->block));

			if (le32toh(rep->seq) == count - 1) {
				csp_buffer_free(packet);
				break;
			}

			csp_buffer_free(packet);

			if (cb != NULL)
				cb(btp, cbarg);

			if (btp_client_finished(btp))
				break;
		}
	}

	return received_data ? BTP_EOK : BTP_ETIMEDOUT;
}

static int btp_client_read_block(struct btp_context *btp, uint32_t block, uint8_t *buf, uint32_t bytes)
{
	if (block > btp->block_status.blocks)
		return BTP_EINVAL;

	if (f_seek(btp->fh, block * btp->block_status.block_size, SEEK_SET)!=0)
		return BTP_EIO;

	if (f_read(buf, 1, bytes, btp->fh) != bytes)
		return BTP_EIO;

	return BTP_EOK;
}

int btp_client_send_blocks(struct btp_context *btp, unsigned int offset, unsigned int count, btp_progress_cb cb, void *cbarg)
{
	int i;
	bool only_missing = false;
	uint32_t remain, bytes;
	struct btp_blockrep *rep;
	csp_packet_t *packet;

	if (!btp || btp->state != STATE_UPLOAD)
		return BTP_EINVAL;

	if (offset == BTP_OFFSET_NEXT) {
		offset = btp->block_status.next;
		only_missing = true;
	}

	for (i = 0; i < count && offset < btp->block_status.blocks; i++) {
		packet = csp_buffer_get(sizeof(*rep) + btp->block_status.block_size);
		if (!packet) {
			UPDEBUG("Failed to get buffer\n");
			return BTP_ENOMEM;
		}

		rep = (struct btp_blockrep *) packet->data;
		rep->type = BTP_BLOCK_REPLY;
		rep->err = BTP_EOK;
		rep->seq = htole32(i);
		rep->total = htole32(count);
		rep->block = htole32(offset++);

		/* Skip forward to next missing block */
		if ((offset - btp->block_status.next) < (BTP_BITFIELD_LENGTH * BITS_PER_BYTE)) {
			while (only_missing &&
			       get_bit(btp->block_status.block_status, offset - btp->block_status.next) &&
			       offset + 1 < btp->block_status.blocks) {
				offset++;
			}
		}

		remain = btp->size - le32toh(rep->block) * btp->block_status.block_size;
		bytes = remain >= btp->block_status.block_size ? btp->block_status.block_size : remain;

		if (btp_client_read_block(btp, le32toh(rep->block), rep->data, bytes) == BTP_EOK) {
			packet->length = sizeof(*rep) + bytes;
			UPDEBUG("sending block %"PRIu32"\n", le32toh(rep->block));
			csp_send(btp->conn, packet);  /* csp_send(...) does not return anything */
		} else {
			UPDEBUG("Failed to read block %"PRIu32"\n", le32toh(rep->block));
			csp_buffer_free(packet);
			return BTP_EIO;
		}
	}

	return BTP_EOK;
}

int btp_client_list(struct btp_context *btp, uint16_t count, const char *backend, const char *name, btp_list_cb cb, void *cbarg)
{
	int i;
	struct btp_listreq req;
	struct btp_listrep rep;
	struct btp_entryreq *ereq;
	struct btp_entryrep *erep;
	csp_packet_t *packet;

	if (!btp || btp->state != STATE_CONNECTED)
		return BTP_EINVAL;

	req.type = BTP_LIST_REQUEST;
	strncpy(req.backend, backend, sizeof(req.backend) - 1);
	req.backend[sizeof(req.backend) - 1] = '\0';
	strncpy(req.name, name, sizeof(req.name) - 1);
	req.name[sizeof(req.name) - 1] = '\0';
	req.count = htole16(count);

	if (csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), &rep, sizeof(rep)) == 0)
		return BTP_ETIMEDOUT;

	if (rep.err != BTP_EOK)
		return rep.err;

	for (i = 0; i < rep.entries; i++) {
		packet = csp_read(btp->conn, btp->timeout);
		if (!packet)
			return BTP_ETIMEDOUT;

		erep = (struct btp_entryrep *) packet->data;

		if (erep->type != BTP_ENTRY_REPLY) {
			csp_buffer_free(packet);
			return BTP_EPROTO;
		}

		erep->size = le32toh(erep->size);
		erep->mode = le32toh(erep->mode);
		erep->etype = le32toh(erep->etype);
		erep->uid = le16toh(erep->uid);
		erep->gid = le16toh(erep->gid);
		erep->nlink = le16toh(erep->nlink);
		erep->mtime = le32toh(erep->mtime);

		if (cb(erep, cbarg) < 0) {
			csp_buffer_free(packet);
			return BTP_EINVAL;
		}

		csp_buffer_free(packet);

		if (i && (i + 1) % count == 0) {
			packet = csp_buffer_get(sizeof(*ereq));
			if (!packet)
				return BTP_ENOMEM;

			ereq = (struct btp_entryreq *) packet->data;

			ereq->type = BTP_ENTRY_REQUEST;
			ereq->count = htole16(count);

			packet->length = sizeof(*ereq);

			csp_send(btp->conn, packet);
		}
	}

	return rep.err;
}

int btp_client_remove(struct btp_context *btp, const char *backend, const char *path)
{
	struct btp_removereq req;
	struct btp_removerep rep;

	if (!btp || btp->state != STATE_CONNECTED)
		return BTP_EINVAL;

	req.type = BTP_REMOVE_REQUEST;
	strncpy(req.backend, backend, sizeof(req.backend) - 1);
	req.backend[sizeof(req.backend) - 1] = '\0';
	strncpy(req.name, path, sizeof(req.name) - 1);
	req.name[sizeof(req.name) - 1] = '\0';

	if (csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), &rep, sizeof(rep)) == 0)
		return BTP_ETIMEDOUT;

	return rep.err;
}

int btp_client_move(struct btp_context *btp, const char *backend, const char *frompath, const char *topath)
{
	struct btp_movereq req;
	struct btp_moverep rep;

	if (!btp || btp->state != STATE_CONNECTED)
		return BTP_EINVAL;

	req.type = BTP_MOVE_REQUEST;
	strncpy(req.backend, backend, sizeof(req.backend) - 1);
	req.backend[sizeof(req.backend) - 1] = '\0';
	strncpy(req.from, frompath, sizeof(req.from) - 1);
	req.from[sizeof(req.from) - 1] = '\0';
	strncpy(req.to, topath, sizeof(req.to) - 1);
	req.to[sizeof(req.to) - 1] = '\0';

	if (csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), &rep, sizeof(rep)) == 0)
		return BTP_ETIMEDOUT;

	return rep.err;
}

int btp_client_copy(struct btp_context *btp, const char *backend, const char *frompath, const char *topath)
{
	struct btp_copyreq req;
	struct btp_copyrep rep;

	if (!btp || btp->state != STATE_CONNECTED)
		return BTP_EINVAL;

	req.type = BTP_COPY_REQUEST;
	strncpy(req.backend, backend, sizeof(req.backend) - 1);
	req.backend[sizeof(req.backend) - 1] = '\0';
	strncpy(req.from, frompath, sizeof(req.from) - 1);
	req.from[sizeof(req.from) - 1] = '\0';
	strncpy(req.to, topath, sizeof(req.to) - 1);
	req.to[sizeof(req.to) - 1] = '\0';

	if (csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), &rep, sizeof(rep)) == 0)
		return BTP_ETIMEDOUT;

	return rep.err;
}





int btp_client_mkdir(struct btp_context *btp, const char *backend, const char *path)
{
	struct btp_mkdirreq req;
	struct btp_mkdirrep rep;

	if (!btp || btp->state != STATE_CONNECTED)
		return BTP_EINVAL;

	req.type = BTP_MKDIR_REQUEST;
	strncpy(req.backend, backend, sizeof(req.backend) - 1);
	req.backend[sizeof(req.backend) - 1] = '\0';
	strncpy(req.name, path, sizeof(req.name) - 1);
	req.name[sizeof(req.name) - 1] = '\0';

	if (csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), &rep, sizeof(rep)) == 0)
		return BTP_ETIMEDOUT;

	return rep.err;
}

int btp_client_rmdir(struct btp_context *btp, const char *backend, const char *path)
{
	struct btp_rmdirreq req;
	struct btp_rmdirrep rep;

	if (!btp || btp->state != STATE_CONNECTED)
		return BTP_EINVAL;

	req.type = BTP_RMDIR_REQUEST;
	strncpy(req.backend, backend, sizeof(req.backend) - 1);
	req.backend[sizeof(req.backend) - 1] = '\0';
	strncpy(req.name, path, sizeof(req.name) - 1);
	req.name[sizeof(req.name) - 1] = '\0';

	if (csp_transaction_persistent(btp->conn, btp->timeout, &req, sizeof(req), &rep, sizeof(rep)) == 0)
		return BTP_ETIMEDOUT;

	return rep.err;
}

int btp_client_complete(struct btp_context *btp, uint32_t timeout_csum)
{
	struct btp_completerequest req;
	struct btp_completereply rep;
	uint32_t checksum;

	if (!btp || (btp->state != STATE_UPLOAD && btp->state != STATE_DOWNLOAD))
		return BTP_EINVAL;

	req.type = BTP_COMPLETE_REQUEST;

	/* Truncate */
	if (btp->state == STATE_DOWNLOAD && btp->block_status.complete) {
		if (f_ftruncate(btp->fh, btp->size)!=0)
			return BTP_EIO;

		if (btp_crc32c_fd(btp->fh, &checksum, 0) < 0)
			return BTP_EIO;

		f_flush(btp->fh);
		f_close(btp->fh);
		btp->fh = 0;

		if (strlen(btp->filename_partial) > 0) {
			if (rename(btp->filename_partial, btp->filename) < 0)
				return BTP_EIO;
		}
		if (btp->checksum != checksum)
			return BTP_ESTALE;
	}

	if (csp_transaction_persistent(btp->conn, timeout_csum, &req, sizeof(req), &rep, sizeof(rep)) == 0)
		return BTP_ETIMEDOUT;

	if (btp->fh > 0)
		f_close(btp->fh);

	return BTP_EOK;
}
