#include <stdio.h>
#include "../util.h"
#include <string.h>

int x = 0;

const char*
driving(void)
{
	char string[] = "[Driving in Progress";
	int n = strlen(string);
	for(int i = 0; i < x; i++)
		string[n++]  = '.';
	string[n++] = ']';
	string[n] = 0;
	x++;
	if(x == 4)
		x = 0;
	return bprintf("                                                                                                    %s", string);
}
