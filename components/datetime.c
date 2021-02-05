/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../util.h"

const char *
datetime(const char *fmt)
{
	time_t t;
	t = time(NULL);
	char s[64] = { 0 };		
	if (!strftime(s, sizeof(buf), fmt, localtime(&t))) {
		warn("strftime: Result string exceeds buffer size");
		return NULL;
	}

//	return buf;
//	return bprintf("^c#%06x^%s^d^", rand()&0xffffff, s);
	return bprintf("%s", s);
}
