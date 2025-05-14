/*
 * Copyright (c) 2013-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#ifndef _SL_BTP_ERROR_H_
#define _SL_BTP_ERROR_H_

/** BTP error codes */
enum btp_errcode {
	BTP_EOK = 0,		/**< No error */
	BTP_EINVAL,		/**< Invalid argument */
	BTP_ENOENT,		/**< No such file or directory */
	BTP_ENOSYS,		/**< Function not implemented */
	BTP_ETIMEDOUT,		/**< Operation timed out */
	BTP_EPROTO,		/**< Protocol error */
	BTP_ENXIO,		/**< No such device or address */
	BTP_EIO,		/**< Input/output error */
	BTP_ENOMEM,		/**< Not enough space */
	BTP_ESTALE,		/**< Stale file handle */
	BTP_EUNKNOWN,		/**< Unknown error */
	BTP_EEXIST,		/**< File exists */
	BTP_EINTR,              /**< Keyboard interrupt */
	BTP_EDISCON= 128,	/**< Disconnect */
};

/**
 * @brief Convert BTP error to string
 * @param err BTP error code
 * @return Pointer to string representation of error, or "Unknown error"
 */
const char *btp_error(enum btp_errcode err);

/**
 * @brief Convert errno value to BTP error
 * @param erno errno value
 * @returns BTP error value or BTP_EUNKNOWN
 */
enum btp_errcode btp_errno_to_error(int erno);

#endif /* _BTP_ERROR_H_ */
