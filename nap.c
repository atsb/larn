/* nap.c */
/* Cleaned up this mess ~Gibbon */
#include <signal.h>
#include <sys/types.h>

#ifdef SYSV
#include <sys/times.h>
#endif

#ifdef BSD
#include <sys/timeb.h>
#endif

/*
 *  routine to take a nap for n milliseconds
 */
nap(x)
    register int x;
    {
    if (x<=0) return(0); /* eliminate chance for infinite loop */
    lflush();
    if (x > 999) sleep(x/1000); else napms(x);
    }

#ifdef NONAP
static napms(x)    /* do nothing */
    int x;
    {
    }
#endif

#ifdef SYSV
/*  napms - sleep for time milliseconds - uses times() */
/* this assumes that times returns a relative time in 60ths of a second */
/* this will do horrible things if your times() returns seconds! */

napms(time)
    int time;
    {
    long matchclock, times();
    struct tms stats;

    if (time<=0) time=1; /* eliminate chance for infinite loop */
    if ((matchclock = times(&stats)) == -1 || matchclock == 0)
        return(0); /* error, or BSD style times() */
    matchclock += (time / 17);      /*17 ms/tic is 1000 ms/sec / 60 tics/sec */

    while(matchclock < times(&stats))
        ;
    }
#endif

#ifdef BSD
#ifdef SIGVTALRM
/* This must be BSD 4.2!  */
#include <sys/time.h>
#define bit(_a) (1<<((_a)-1))

static void nullf()
    {
    }

/*  napms - sleep for time milliseconds - uses setitimer() */
napms(time)
    int time;
    {
    struct itimerval    timeout;
#ifdef SIG_RTNS_INT
    int     (*oldhandler) ();
#else
    void     (*oldhandler) ();
#endif
    int     oldsig;

    if (time <= 0) return(0);

    timerclear(&timeout.it_interval);
    timeout.it_value.tv_sec = time / 1000;
    timeout.it_value.tv_usec = (time % 1000) * 1000;

    oldsig = sigblock(bit(SIGALRM));
    setitimer(ITIMER_REAL, &timeout, (struct itimerval *)0);
    oldhandler = signal(SIGALRM, nullf);
    sigpause(oldsig);
    signal(SIGALRM, oldhandler);
    sigsetmask(oldsig);
    }
#endif
#endif
