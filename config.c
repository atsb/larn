/*
 *  config.c    --  This defines the installation dependent variables.
 *                  Some strings are modified later.  ANSI C would
 *                  allow compile time string concatenation, we must
 *                  do runtime concatenation, in main.
 */
/* Removed all MSDOS and VMS stuff. ~Gibbon */
#include "header.h"
#include "larndefs.h"

#ifndef LARNHOME
#define LARNHOME "./"     /* normally supplied by a Makefile */
#endif

#ifndef WIZID
#define WIZID   0
#endif

/*
 *  All these strings will be appended to in main() to be complete filenames
 */
        /* the game save filename   */
char savefilename[SAVEFILENAMESIZE] =   LARNHOME;

        /* the score file           */
char scorefile[sizeof(LARNHOME)+sizeof(SCORENAME)] =    LARNHOME;

        /* the logging file         */
char logfile[sizeof(LARNHOME)+sizeof(LOGFNAME)]  =      LARNHOME;

        /* the help text file       */
char helpfile[sizeof(LARNHOME)+sizeof(HELPNAME)] =      LARNHOME;

        /* the maze data file       */
char larnlevels[sizeof(LARNHOME)+sizeof(LEVELSNAME)] =  LARNHOME;

        /* the fortune data file    */
char fortfile[sizeof(LARNHOME)+sizeof(FORTSNAME)] =     LARNHOME;

        /* the .larnopts filename */
char optsfile[128];             /* the option file          */

        /* the player id datafile name */
char playerids[sizeof(LARNHOME)+sizeof(PLAYERIDS)] =    LARNHOME;

#ifdef TIMECHECK
        /* the holiday datafile */
char holifile[sizeof(LARNHOME)+sizeof(HOLIFILE)] =      LARNHOME;
#endif

char ckpfile[sizeof(LARNHOME)+sizeof(CKPFILE)] = LARNHOME;

#ifdef EXTRA
char diagfile[] ="Diagfile";        /* the diagnostic filename  */
#endif

char *password ="lotg";    /* the wizards password <=32*/
#if WIZID == -1
int wisid=0;            /* the user id of the only person who can be wizard */
#else
int wisid=WIZID;        /* the user id of the only person who can be wizard */
#endif
char psname[PSNAMESIZE]="larn"; /* the process name     */
