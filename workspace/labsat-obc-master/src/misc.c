/*- misc functions (to prevent include non-freertos libs ) ----------------------------------*/
#include "misc.h"

char tolc(char c) {
	 return ( c>='A' && c<='Z' ) ? (c|' ') : c;
}
char icasecmp(const char* a, const char* b, int len) {
	int n;
	for(n=0; n<len; ++n)
		if( tolc(*a)!=tolc(*b) ) return 0;
	return 1;
}

