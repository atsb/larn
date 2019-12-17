/*  header.h        Larn is copyrighted 1986 by Noah Morgan. */

#include <stdio.h>
#include <stdlib.h>

#define LARNHOME ""

#ifndef WIZID
#   define WIZID  1000
#endif

#define TRUE 1
#define FALSE 0

#define SCORENAME   "larn.scr"
#define LOGFNAME    "larn.log"
#define HELPNAME    "larn.hlp"
#define LEVELSNAME  "larn.maz"
#define FORTSNAME   "larn.ftn"
#define PLAYERIDS   "larn.pid"
#define HOLIFILE    "larn.hol"
#define LARNOPTS    "larn.opt"
#define SAVEFILE    "larn.sav"
#define CKPFILE     "larn.ckp"
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

/* PROTOTYPES FOR SOURCE FILES */

/* ACTION.C */
int act_remove_gems(int);
int act_sit_throne(int);
int act_up_stairs(void);
int act_down_stairs(void);
int act_drink_fountain(void);
int act_wash_fountain(void);
int act_down_shaft(void);
int act_up_shaft(void);
int volshaft_climbed(int);
int act_desecrate_altar(void);
int act_donation_pray(void);
int act_just_pray(void);
void act_prayer_heard(void);
int act_ignore_altar(void);
int act_open_chest(int,int);
int act_open_door(int,int);

/* IO.C */
void lprcat(char*);
void newgame(void);
void lprintf(char *fmt, ...);
long larnlrint(void);
void cursor(int,int);
void cursors(void);
int sncbr(void);

/* SAVELEV.C /*
void savelevel(void);
void getlevel(void);

/* GLOBAL.C */
int rnd (int);
int rund (int);
