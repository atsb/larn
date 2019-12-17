/* tok.c */
/* Removed all the cruft and made it SYSV and BSD only. ~Gibbon */
/*
   yylex()
   flushall()
   sethard()
   readopts()
   actual_readopts()
*/
#include <sys/types.h>

#if defined SYSV || BSD
#include <fcntl.h>
#include <termio.h>
#include <sys/ioctl.h>
#endif

#include <ctype.h>
#include "header.h"
#include "larndefs.h"
#include "monsters.h"
#include "objects.h"
#include "player.h"

static void actual_readopts();

#define CHKPTINT   400

static char lastok=0;
int yrepcount=0;
char move_no_pickup = FALSE ;

#ifdef TIMECHECK
int dayplay=0;
#endif

#ifndef FLUSHNO
#define FLUSHNO 5
#endif

static int flushno=FLUSHNO; /* input queue flushing threshold */
#define MAXUM 52    /* maximum number of user re-named monsters */
#define MAXMNAME 40 /* max length of a monster re-name */
static char usermonster[MAXUM][MAXMNAME]; /* the user named monster name goes here */
static char usermpoint=0;           /* the user monster pointer */

/*
    lexical analyzer for larn
 */
yylex()
    {
    char cc;
    int ic;
    char firsttime = TRUE;

    if (hit2flag)
        {
        hit2flag=0;
        yrepcount=0;
        return(' ');
        }
    if (yrepcount>0)
        {
        --yrepcount;
        return(lastok);
        }
    else
        yrepcount=0;
    if (yrepcount==0) 
        { 
        bottomdo(); 
        showplayer();               /* show where the player is */
        move_no_pickup = FALSE;     /* clear 'm' flag */
        }

    lflush();
    while (1)
        {
        c[BYTESIN]++;
        if (ckpflag)
          if ((c[BYTESIN] % CHKPTINT) == 0) /* check for periodic checkpointing */
            {
#ifndef DOCHECKPOINTS
            wait(0);    /* wait for other forks to finish */
            if (fork() == 0) { savegame(ckpfile); exit(0); }
#endif

#ifdef TIMECHECK
            if (dayplay==0)
              if (playable())
                {
                cursor(1,19);
                lprcat("\nSorry, but it is now time for work.  Your game has been saved.\n"); beep();
                lflush();
                savegame(savefilename);
                wizard=nomove=1;
                sleep(4);
                died(-257);
                }
#endif
            }

#if defined SYSV || BSD
        do /* if keyboard input buffer is too big, flush some of it */
            {
            ioctl(0,FIONREAD,&ic);
            if (ic>flushno)   read(0,&cc,1);
            }
        while (ic>flushno);
#endif

        if (read(0,&cc,1) != 1)
            return(lastok = -1);
        if (cc == '!')      /* ! shell escape */
            {
            resetscroll();  /* scrolling region, home, clear, no attributes */
            clear();
            lflush();
            sncbr();
            if ((ic=fork())==0) /* child */
                {
#ifdef SYSV
                char *s, *getenv();
                if ((s=getenv("SHELL")) == (char *) 0)
                    s = "/bin/sh";
                execl(s,"larn-shell", (char *)0);
                exit(0);
#else
                execl("/bin/csh",0);
                exit(0);
                wait(0);
#endif
                }
            if (ic<0) /* error */
                {
                write(2,"Can't fork off a shell!\n",25);
                sleep(2);
                }
#ifdef SYSV
            else
                wait( (int *)0 );
#endif
            setscroll();
            return(lastok = 'L'-64);    /* redisplay screen */
            }

        /* get repeat count, showing to player
        */
        if ((cc <= '9') && (cc >= '0'))
            {
            yrepcount = yrepcount*10 + cc - '0';

            /* show count to player for feedback
            */
            if ( yrepcount >= 10 )
                {
                cursors();
                if (firsttime)
                    lprcat("\n");
                lprintf("count: %d", (long)yrepcount );
                firsttime=FALSE;
                lflush();  /* show count */
                }
            }
        else
            {
            /* check for multi-character commands in command mode, and handle.
            */
            if ( cc == 'm' && !prompt_mode )
                {
                move_no_pickup = TRUE ;
                cc = ttgetch();
                }
            if ( yrepcount > 0 )
                --yrepcount;
            return(lastok = cc);
            }
        }
    }

/*
 *  flushall()  Function to flush all type-ahead in the input buffer
 */
lflushall()
    {
#ifdef SYSV
    ioctl(0,TCFLSH,0);
#else
    char cc;
    int ic;
    for (;;) {      /* if keyboard input buffer is too big, flush some of it */
        ioctl(0,FIONREAD,&ic);
        if (ic<=0)
            return(0);
        while (ic>0) {
            read(0,&cc,1);
            --ic;
        } /* gobble up the byte */
    }
#endif
}

/*
    function to set the desired hardness
    enter with hard= -1 for default hardness, else any desired hardness
 */
sethard(hard)
int hard;
{
    register int    j,k;
    long        i;
    struct monst    *mp;

    j=c[HARDGAME]; hashewon();
    if (restorflag==0)  /* don't set c[HARDGAME] if restoring game */
        {
        if (hard >= 0) c[HARDGAME]= hard;
        }
    else c[HARDGAME]=j; /* set c[HARDGAME] to proper value if restoring game */

    if (k=c[HARDGAME])
      for (j=0; j<=MAXMONST+8; j++) {
        mp = &monster[j];
        i = ((6+k) * mp->hitpoints + 1)/6;
        mp->hitpoints = (i<0) ? 32767 : i;
        i = ((6+k) * mp->damage + 1) / 5;
        mp->damage = (i>127) ? 127 : i;
        i = (10 * mp->gold)/(10+k);
        mp->gold = (i>32767) ? 32767 : i;
        i = mp->armorclass - k;
        mp->armorclass = (i< -127) ? -127 : i;
        i = (7*mp->experience)/(7+k) + 1;
        mp->experience = (i<=0) ? 1 : i;
    }
}


/*
    function to read and process the larn options file
*/
readopts()
    {
    register int j;
    char original_objects = FALSE ;
    char namenotchanged = TRUE;     /* set to 0 if a name is specified */

    if (lopen(optsfile) < 0)
        {
        lprintf("Can't open options file \"%s\"\n", optsfile);
        lflush();
        sleep(1);
        }
    else
        actual_readopts(&namenotchanged, &original_objects);

    if (namenotchanged)
        strcpy(logname,loginname);

    /* original objects require object highlighting to be ON (in order
       to distinguish between objects and monsters).  set up object list
       properly.
    */
    if (original_objects)
        {
        boldobjects = TRUE ;
        strncpy( objnamelist, original_objnamelist, MAXOBJECT );
        }
    else
        strncpy( objnamelist, hacklike_objnamelist, MAXOBJECT );
    objnamelist[MAXOBJECT] = '\0' ;

    /* now set all the invisible objects and monsters to have the
       same appearance as the floor (as defined by the user)
    */
    objnamelist[OWALL] = wallc;

    objnamelist[0]        =
    objnamelist[OIVTELETRAP]  =
    objnamelist[OTRAPARROWIV] =
    objnamelist[OIVDARTRAP]   =
    objnamelist[OIVTRAPDOOR]  = floorc;
    monstnamelist[0] =
    monstnamelist[INVISIBLESTALKER] = floorc;
    for (j=DEMONLORD; j<=DEMONPRINCE; j++)
        monstnamelist[j] = floorc;
    }

static void actual_readopts( namenotchanged, original_objects )
char *namenotchanged;
char *original_objects;
    {
    register char *i;
    register int j,k;

    i = " ";
    while (*i)
        {
        /* check for EOF
        */
        if ((i=(char *)lgetw()) == 0)
            return;
#if 0
        while (*i && ((*i==' ') || (*i=='\t'))) i++; /* eat leading whitespace */
#endif
        /* leading # a comment, eat the rest of the line.  Handle multiple
           comment lines in a row.
        */
        while (*i == '#')
            {
            char cc;
            do
                cc = (char)lgetc();
            while ( ( cc != '\n' ) && ( cc != NULL));
            if ((i = (char *)lgetw()) == 0)
                return;
            }

        if (strcmp(i,"bold-objects") == 0)
            boldon=1;
        else if (strcmp(i,"enable-checkpointing") == 0)
            ckpflag=1;
        else if (strcmp(i,"inverse-objects") == 0)
            boldon=0;
        else if (strcmp(i,"prompt-on-objects") == 0 )
            prompt_mode = TRUE ;
        else if (strcmp(i,"auto-pickup") == 0 )
            auto_pickup = TRUE ;
        else if (strcmp(i,"highlight-objects") == 0 )
            boldobjects = TRUE ;
        else if (strcmp(i,"original-objects") == 0 )
            *original_objects = TRUE ;
        else if (strcmp(i,"female") == 0)
            sex=0; /* male or female */
        else if (strcmp(i,"monster:")== 0)   /* name favorite monster */
            {
            if ((i=(char *)lgetw())==0)
                break;
            if (strlen(i)>=MAXMNAME)
                i[MAXMNAME-1]=0;
            strcpy(usermonster[usermpoint],i);
            if (usermpoint >= MAXUM)
                break; /* defined all of em */
            if (isalpha(j=usermonster[usermpoint][0]))
                {
                for (k=1; k<MAXMONST+8; k++) /* find monster */
                  if (monstnamelist[k] == j)
                    {
                    monster[k].name = &usermonster[usermpoint++][0];
                    break;
                    }
                }
            }
        else if (strcmp(i,"male") == 0)
            sex=1;
        else if (strcmp(i,"name:") == 0) /* defining players name */
            {
            if ((i=lgetw())==0)
                break;
            if (strlen(i)>=LOGNAMESIZE)
                i[LOGNAMESIZE-1]=0;
            strcpy(logname,i);
            *namenotchanged = FALSE;
            }
        else if (strcmp(i,"no-introduction") == 0)
            nowelcome=1;
        else if (strcmp(i,"no-beep") == 0)
            nobeep=1;
        else if (strcmp(i,"process-name:")== 0)
            {
            if ((i=lgetw())==0)
                break;
            if (strlen(i)>=PSNAMESIZE)
                i[PSNAMESIZE-1]=0;
            strcpy(psname,i);
            }
        else if (strcmp(i,"play-day-play") == 0)
#ifdef TIMECHECK
            dayplay=1;
#else
        ;
#endif
        else if (strcmp(i,"savefile:") == 0) /* defining savefilename */
            {
            if ((i=lgetw())==0)
                break;
            if (strlen(i)>=SAVEFILENAMESIZE) /* avoid overflow */
                i[SAVEFILENAMESIZE-1]=0;
            strcpy(savefilename,i);
            }
        else
            {
            lprintf("Unknown option \"%s\"\n", i);
            lflush();
            sleep(1);
            }
        }
    }
