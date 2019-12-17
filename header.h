/*  header.h        Larn is copyrighted 1986 by Noah Morgan. */

#include <stdio.h>
#include <stdlib.h>

#define LARNHOME ""

#ifndef WIZID
#   define WIZID  1000
#endif

#define TRUE 1
#define FALSE 0

#define SCORENAME   "data/larn.scr"
#define LOGFNAME    "data/larn.log"
#define HELPNAME    "data/larn.hlp"
#define LEVELSNAME  "data/larn.maz"
#define FORTSNAME   "data/larn.ftn"
#define PLAYERIDS   "data/larn.pid"
#define HOLIFILE    "data/larn.hol"
#define LARNOPTS    "data/larn.opt"
#define SAVEFILE    "data/larn.sav"
#define CKPFILE     "data/larn.ckp"
#define MAIL     /* disable the mail routines for MSDOS */

#define MAXLEVEL 11    /*  max # levels in the dungeon         */
#define MAXVLEVEL 3    /*  max # of levels in the temple of the luran  */
#define MAXX 68
#define MAXY 17

#define SCORESIZE 10    /* this is the number of people on a scoreboard max */
#define MAXPLEVEL 100   /* maximum player level allowed        */
#define SPNUM 38        /* maximum number of spells in existance   */
#define TIMELIMIT 30000 /* maximum number of moves before the game is called */
#define TAXRATE 1/20    /* tax rate for the LRS */


/*  this is the structure that holds the entire dungeon specifications  */
struct cel
    {
    short   hitp;   /*  monster's hit points    */
    char    mitem;  /*  the monster ID          */
    char    item;   /*  the object's ID         */
    short   iarg;   /*  the object's argument   */
    char    know;   /*  have we been here before*/
    };

/* this is the structure for maintaining & moving the spheres of annihilation */
struct sphere
    {
    struct sphere *p;   /* pointer to next structure */
    char x,y,lev;       /* location of the sphere */
    char dir;           /* direction sphere is going in */
    char lifetime;      /* duration of the sphere */
    };

#define BUFBIG  4096            /* size of the output buffer */
#define MAXIBUF 4096            /* size of the input buffer */
#define LOGNAMESIZE 40          /* max size of the players name */
#define PSNAMESIZE 40           /* max size of the process name */
#define SAVEFILENAMESIZE 128    /* max size of the savefile path */
