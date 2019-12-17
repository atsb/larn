/* nap.c */
/* Cleaned up this mess ~Gibbon */
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>
#include "nap.h"

/*  napms - sleep for time milliseconds - uses times() */
/* this assumes that times returns a relative time in 60ths of a second */
/* this will do horrible things if your times() returns seconds! */

int napms(int time)
{
	long matchclock, times();
	struct tms stats;
	if (time<=0)
	{
		time=1; /* eliminate chance for infinite loop */
	}
	if ((matchclock = times(&stats)) == -1 || matchclock == 0)
	{
        	return(0); /* error */
        }
        matchclock += (time / 17);      /*17 ms/tic is 1000 ms/sec / 60 tics/sec */
        while(matchclock < times(&stats));
        return(0);
}

/*
 *  routine to take a nap for n milliseconds
 */
int nap(int x)
{
	if (x<=0)
	{	
		return(0); /* eliminate chance for infinite loop */
		lflush();
	}
	if (x > 999)
	{
		sleep(x/1000);
	}
	else
	{
		napms(x);
	}
	return(0);
}
