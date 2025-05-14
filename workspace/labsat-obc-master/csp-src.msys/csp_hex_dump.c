
#include "LogManager.h"
#include <at91/peripherals/dbgu/dbgu.h>
#include <inttypes.h>
#include <csp/csp_debug.h>
#include <stddef.h>

#if !defined(FLIGHT_VERSION) && CSP_ENABLE_CSP_PRINT && COMPILE_LOGLEVEL>=7
void csp_hex_dump_format(const char * desc, void * addr, int len, int format) {
	if( runLogLevel<7 ) return;
	int i;
	unsigned char buff[17];
	unsigned char * pc = (unsigned char *)addr;
	char logbuf[MAXLOGLINE];
	unsigned int n = 0;

	// Output description if given.
	if (desc != NULL)
		n += snprintf(logbuf,MAXLOGLINE-1,"%s\n", desc);

	if (!(len > 0))
		return;

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				n += snprintf(logbuf+n,MAXLOGLINE-1-n,"  %s\n", buff);

			// Output the offset.
			if (format & 0x1) {
				n += snprintf(logbuf+n,MAXLOGLINE-1-n,"  %p ", ((uint8_t *)addr) + i);
			} else {
				n += snprintf(logbuf+n,MAXLOGLINE-1-n,"        ");
			}
		}

		// Now the hex code for the specific character.
		n += snprintf(logbuf+n,MAXLOGLINE-1-n," %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		n += snprintf(logbuf+n,MAXLOGLINE-1-n,"   ");
		i++;
	}

	// And print the final ASCII bit.
	n += snprintf(logbuf+n,MAXLOGLINE-1-n,"  %s\n", buff);
	logbuf[MAXLOGLINE-1]=0;
	UPLOG_DEBUG(logbuf);
}

void csp_hex_dump(const char * desc, void * addr, int len) {
	csp_hex_dump_format(desc, addr, len, 0);
}
#endif
