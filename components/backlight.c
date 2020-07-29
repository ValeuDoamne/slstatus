#include <stdio.h>
#include <string.h>

#include "../util.h"

#include <limits.h>
#include <stdint.h>
#include <unistd.h>

const char *
backlight(const char *dsp)
{
	int perc;
	char path[PATH_MAX];
	
	if (esnprintf(path, sizeof(path),
				"/sys/class/backlight/%s/brightness", dsp) < 0){
		return NULL;
	}
	if(pscanf(path, "%d", &perc) != 1){
		return NULL;
	}
	return bprintf("%d", (int)((float)(perc/255.0*100))); // casting feng shui (just get something between 0 100)
}

