/*
 * Copyright (c) 2013-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <errno.h>

#include <btp/error.h>

const char *btp_errstring[] = {
	[BTP_EOK] = "No error",
	[BTP_EINVAL] = "Invalid argument",
	[BTP_ENOENT] = "No such file or directory",
	[BTP_ENOSYS] = "Function not implemented",
	[BTP_ETIMEDOUT] = "Operation timed out",
	[BTP_EPROTO] = "Protocol error",
	[BTP_ENXIO] = "No such device or address",
	[BTP_EIO] = "Input/output error",
	[BTP_ENOMEM] = "Not enough space",
	[BTP_ESTALE] = "Stale file handle",
	[BTP_EEXIST] = "File exists",
	[BTP_EINTR] = "Keyboard Interrupt",
	[BTP_EUNKNOWN] = "Unknown error",
};

const char *btp_error(enum btp_errcode err)
{
	err &= ~BTP_EDISCON;
	if (err < sizeof(btp_errstring) / sizeof(btp_errstring[0]))
		return btp_errstring[err];
	else
		return "Unknown error";
}

#define DIRECT_MAP_ERRNO(erno) case erno: return BTP_ ## erno

enum btp_errcode btp_errno_to_error(int erno)
{
	switch (erno) {
		DIRECT_MAP_ERRNO(EINVAL);
		DIRECT_MAP_ERRNO(ENOENT);
		DIRECT_MAP_ERRNO(ENOSYS);
		DIRECT_MAP_ERRNO(ETIMEDOUT);
		DIRECT_MAP_ERRNO(EPROTO);
		DIRECT_MAP_ERRNO(ENXIO);
		DIRECT_MAP_ERRNO(EIO);
		DIRECT_MAP_ERRNO(ENOMEM);
		DIRECT_MAP_ERRNO(ESTALE);
		DIRECT_MAP_ERRNO(EEXIST);
		DIRECT_MAP_ERRNO(EINTR);
		default:
			return BTP_EUNKNOWN;
	}
}
