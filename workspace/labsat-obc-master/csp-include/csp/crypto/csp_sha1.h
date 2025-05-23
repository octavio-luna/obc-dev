#pragma once

/**
   @file
   SHA1 support.

   Code originally from Python's SHA1 Module, who based it on libtom.org.
*/

#include <csp/csp_types.h>
#include <csp_autoconfig.h>



/** The SHA1 block size in bytes */
#define CSP_SHA1_BLOCKSIZE	64

/** The SHA1 digest (hash) size in bytes */
#define CSP_SHA1_DIGESTSIZE	20

/**
   SHA1 state.
*/
typedef struct {
        //! Internal SHA1 state.
	uint64_t length;
        //! Internal SHA1 state.
	uint32_t state[5];
        //! Internal SHA1 state.
	uint32_t curlen;
        //! Internal SHA1 state.
	uint8_t  buf[CSP_SHA1_BLOCKSIZE];
} csp_sha1_state_t;

/**
   Initialize the hash state
   @param[in] state hash state.
*/
void csp_sha1_init(csp_sha1_state_t * state);

/**
   Process a block of memory through the hash.
   @param[in] state hash state
   @param[in] data data.
   @param[in] length length of \a data.
*/
void csp_sha1_process(csp_sha1_state_t * state, const void * data, uint32_t length);

/**
   Terminate the hash calculation and get the SHA1.
   @param[in] state hash state
   @param[out] sha1 user supplied buffer of minimum #CSP_SHA1_DIGESTSIZE bytes.
*/
void csp_sha1_done(csp_sha1_state_t * state, uint8_t * sha1);

/**
   Calculate SHA1 hash of block of memory.
   @param[in] data data.
   @param[in] length length of \a data.
   @param[out] sha1 user supplied buffer of minimum #CSP_SHA1_DIGESTSIZE bytes.
*/
void csp_sha1_memory(const void * data, uint32_t length, uint8_t * sha1);
