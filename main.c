/* main.c */

#include <pwd.h>

#include "header.h"
#include "larndefs.h"
#include "monsters.h"
#include "objects.h"
#include "player.h"
#include "patchlev.h"

extern char move_no_pickup;
int dropflag=0; /* if 1 then don't lookforobject() next round */
int rmst=80;    /*  random monster creation counter     */
int userid;     /* the players login user id number */
char nowelcome=0,nomove=0; /* if (nomove) then don't count next iteration as a
                              move */
static char viewflag=0;    /* if viewflag then we have done a 99 stay here
                              and don't showcell in the main loop */
char restorflag=0;         /* 1 means restore has been done    */
char prompt_mode = 0;         /* 1 if prompting for actions */

#ifdef MSDOS

static char cmdhelp[] = "\
Cmd line format: larn [-slicnhp] [-o<optsfile>] [-##] [++]\n\
  -s   show the scoreboard\n\
  -l   show the logfile (wizard id only)\n\
  -i   show scoreboard with inventories of dead characters\n\
  -c   create new scoreboard (wizard id only)\n\
  -n   suppress welcome message on starting game\n\
  -##  specify level of difficulty (example: -5)\n\
  -h   print this help text\n\
  -p   prompt for actions on objects\n\
  ++   restore game from checkpoint file\n\
  -o<optsfile>   specify larnopts filename to be used instead of \"larn.opt\"\n\
";

# else

static char cmdhelp[] = "\
Cmd line format: larn [-slicnhp] [-o<optsfile>] [-##] [++]\n\
  -s   show the scoreboard\n\
  -l   show the logfile (wizard id only)\n\
  -i   show scoreboard with inventories of dead characters\n\
  -c   create new scoreboard (wizard id only)\n\
  -n   suppress welcome message on starting game\n\
  -##  specify level of difficulty (example: -5)\n\
  -h   print this help text\n\
  -p   prompt for actions on objects\n\
  ++   restore game from checkpoint file\n\
  -o<optsfile>   specify .larnopts filename to be used instead of \"~/.larnopts\"\n\
";

# endif

#ifdef MSDOS
int save_mode = 0;      /* 1 if doing a save game */
jmp_buf save_jbuf;      /* To recover from disk full errors */
#endif

#ifdef VT100
static char *termtypes[] = { "vt100", "vt101", "vt102", "vt103", "vt125",
    "vt131", "vt140", "vt180", "vt220", "vt240", "vt241", "vt320", "vt340",
    "vt341"  };
#endif

#ifdef VMS
# define EXIT_FAILURE
# define EXIT_SUCCESS 1
#else
# define EXIT_FAILURE 1
# define EXIT_SUCCESS 0
#endif

/*
    ************
    MAIN PROGRAM
    ************
*/
main(argc,argv)
    int argc;
    char **argv;
    {
    register int i,j;
    int hard = -1;
    char *ptr=0;
    char *homedir = getenv("HOME");
#ifdef VT100
    char *ttype;
#endif
    struct passwd *pwe,*getpwuid();

/*
 *  first task is to identify the player
 */
    init_term();    /* setup the terminal (find out what type) for termcap */
    ptr = "NUFF";

/*
 *  second task is to prepare the pathnames the player will need
 */
    strcpy(loginname,ptr); /* save loginname of the user for logging purposes */
    strcpy(logname,ptr);    /* this will be overwritten with the players name */

/* Set up the input and output buffers.
 */
    lpbuf    = (char *)malloc((5* BUFBIG)>>2);  /* output buffer */
    inbuffer = (char *)malloc((5*MAXIBUF)>>2);  /* output buffer */
    if ((lpbuf==0) || (inbuffer==0)) 
        died(-285); /* malloc() failure */

    strcat(savefilename, SAVEFILE);
    strcat(optsfile, LARNOPTS);   /* the options filename */
    strcat(scorefile, SCORENAME);   /* the larn scoreboard filename */
    strcat(logfile, LOGFNAME);      /* larn activity logging filename */
    strcat(helpfile, HELPNAME);     /* the larn on-line help file */
    strcat(larnlevels, LEVELSNAME); /* the pre-made cave level data file */
    strcat(fortfile, FORTSNAME);    /* the fortune data file name */
    strcat(playerids, PLAYERIDS);   /* the playerid data file name */
    strcat(ckpfile, CKPFILE);

# ifdef TIMECHECK
    strcat(holifile, HOLIFILE);     /* the holiday data file name */
# endif

#ifdef VT100
/*
 *  check terminal type to avoid users who have not vt100 type terminals
 */
    ttype = getenv("TERM");
    for (j=1, i=0; i<sizeof(termtypes)/sizeof(char *); i++)
        if (strcmp(ttype,termtypes[i]) == 0) { j=0;  break; }
    if (j)
        {
        lprcat("Sorry, Larn needs a VT100 family terminal for all it's features.\n"); lflush();
        exit(EXIT_FAILURE);
        }
#endif

/*
 *  now make scoreboard if it is not there (don't clear) 
 */
    if (access(scorefile,0) == -1) /* not there */
        makeboard();

/*
 *  now process the command line arguments 
 */
    for (i=1; i<argc; i++)
        {
        if (argv[i][0] == '-')
          switch(argv[i][1])
            {
            case 's':          /* show scoreboard   */
                showscores();
                exit(EXIT_SUCCESS);

            case 'l':          /* show log file     */
                diedlog();
                exit(EXIT_SUCCESS);

            case 'i':          /* show all scoreboard */
                showallscores();
                exit(EXIT_SUCCESS);

            case 'c':          /* anyone with password can create scoreboard */
                lprcat("Preparing to initialize the scoreboard.\n");
                if (getpassword() != 0)  /*make new scoreboard*/
                    {
                    makeboard();
                    lprc('\n');
                    showscores();
                    }
                exit(EXIT_SUCCESS);

            case 'n':          /* no welcome msg   */
                nowelcome=1;
                argv[i][0]=0;
                break;

            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': /* for hardness */
                hard = atoi(&argv[i][1]);
                break;

            case 'h':          /* print out command line arguments */
            case '?':
                write(1,cmdhelp,sizeof(cmdhelp));
                exit(EXIT_SUCCESS);

            case 'o':          /* specify a .larnopts filename */
                if (argv[i]+2 != '\0')
                    strncpy(optsfile,argv[i]+2,127);
                else
                    {
                    strncpy( optsfile, argv[i+1][0], 127 );
                    argv[i+1][0] = '\0';
                    }
                break;

            case 'p':          /* set 'prompt_mode' flag */
                prompt_mode = 1 ;
                break ;

            default:
                printf("Unknown option <%s>\n",argv[i]);
                write(1,cmdhelp,sizeof(cmdhelp));
                exit(EXIT_SUCCESS);
            };

        if (strcmp(argv[i], "++") == 0)
            restorflag = 1;
    }

#ifndef MSDOS
    readopts();     /* read the options file if there is one */
#endif

#ifdef TIMECHECK
/*
 *  this section of code checks to see if larn is allowed during working hours
 */
    if (dayplay==0) /* check for not-during-daytime-hours */
      if (playable())
        {
        write(2,"Sorry, Larn can not be played during working hours.\n",52);
        exit(EXIT_SUCCESS);
        }
#endif TIMECHECK

#ifdef UIDSCORE
    userid = geteuid(); /* obtain the user's effective id number */
#else UIDSCORE
    userid = getplid(logname);  /* obtain the players id number */
#endif UIDSCORE
#ifdef VMS
    wisid = userid;
#endif
    if (userid < 0) 
        { 
        write(2,"Can't obtain playerid\n",22);
        exit(EXIT_SUCCESS);
        }

#ifdef HIDEBYLINK
/*
 *  this section of code causes the program to look like something else to ps
 */
    if (strcmp(psname,argv[0])) /* if a different process name only */
        {
        if ((i=access(psname,1)) < 0)
            {       /* link not there */
            if (link(argv[0],psname)>=0)
                {
                argv[0] = psname;   execv(psname,argv);
                }
            }
        else
            unlink(psname);
        }

    for (i=1; i<argc; i++)
        {
        szero(argv[i]); /* zero the argument to avoid ps snooping */
        }
#endif HIDEBYLINK

/*
 *  He really wants to play, so malloc the memory for the dungeon.
 */
# ifdef MSDOS
    allocate_memory();
# else
    cell = (struct cel *)malloc(sizeof(struct cel)*(MAXLEVEL+MAXVLEVEL)*MAXX*MAXY);
    if (cell == 0) died(-285);  /* malloc failure */
# endif
    lcreat((char*)0);   
    newgame();      /*  set the initial clock  */

    if (restorflag == 1)           /* restore checkpoint file */
        {
        clear();
        hitflag = 1;
        restoregame(ckpfile);
        }
    if (access(savefilename, 0) == 0) {    /* restore game if need to */
            clear();
            restorflag = 1;
            hitflag = 1;
            restoregame(savefilename);    /* restore last game     */
        }
#ifdef SYSVSIGNALS
    sigsetup();     /* trap all needed signals  */
#else
	sigset();
#endif
    setupvt100();   /*  setup the terminal special mode             */
    sethard(hard);  /* set up the desired difficulty                */
    if (c[HP]==0)   /* create new game */
        {
        makeplayer();   /*  make the character that will play           */
        newcavelevel(0);/*  make the dungeon                            */
        predostuff = 1; /* tell signals that we are in the welcome screen */
        if (nowelcome==0)
            welcome();     /* welcome the player to the game */
# ifdef MSDOS
        /* Display their mail if they've just won the previous game
         */
        checkmail();
# endif
        }

    lprc(T_INIT);   /* Reinit the screen because of welcome and check mail
                     * having embedded escape sequences.*/
    drawscreen();   /*  show the initial dungeon                    */
    predostuff = 2; /* tell the trap functions that they must do a showplayer()
               from here on */
    /* nice(1); /* games should be run niced */
    yrepcount = hit2flag = 0;
    /* init previous player position to be current position, so we don't
       reveal any stuff on the screen prematurely.
    */
    oldx = playerx;
    oldy = playery;

    /* MAINLOOP
       find objects, move stuff, get commands, regenerate
    */
    while (1)
        {
        if (dropflag==0)
            /* see if there is an object here.

               If in prompt mode, identify and prompt; else
               identify, pickup if ( auto pickup and not move-no-pickup ),
               never prompt.
            */
            if (prompt_mode)
                lookforobject( TRUE, FALSE, TRUE );
            else
                lookforobject( TRUE, ( auto_pickup && !move_no_pickup ), FALSE );
            else
                dropflag=0; /* don't show it just dropped an item */

        /* handle global activity
           update game time, move spheres, move walls, move monsters
           all the stuff affected by TIMESTOP and HASTESELF
        */
        if (c[TIMESTOP] <= 0)
            if (c[HASTESELF] == 0 ||
               (c[HASTESELF] & 1) == 0)
                {
                gtime++;
                movsphere();

                if (hitflag==0)
                    {
                    if (c[HASTEMONST])
                        movemonst();
                    movemonst();
                    }
                }

        /* show stuff around the player
        */
        if (viewflag==0)
            showcell(playerx,playery);
        else
            viewflag=0;

        if (hit3flag)
            lflushall();
        hitflag=hit3flag=0;
        bot_linex();    /* update bottom line */

        /* get commands and make moves
        */
        nomove=1;
        while (nomove)
            {
            if (hit3flag)
                lflushall();
            nomove=0;
            parse();
            }
        regen();            /*  regenerate hp and spells            */
        if (c[TIMESTOP]==0)
            if (--rmst <= 0)
                {
                rmst = 120-(level<<2);
                fillmonst(makemonst(level));
                }
        }
    }

/*
    subroutine to randomly create monsters if needed
 */
static randmonst()
    {
    if (c[TIMESTOP]) return(0);    /*  don't make monsters if time is stopped  */
    if (--rmst <= 0)
        {
        rmst = 120 - (level<<2);  fillmonst(makemonst(level));
        }
    }


/*
    parse()

    get and execute a command
 */
parse()
    {
    register int i,j,k,flag;
    extern showeat(),showquaff(),showread();

    while   (1)
        {
        k = yylex();
        switch(k)   /*  get the token from the input and switch on it   */
            {
            case 'h':   moveplayer(4);  return(0);     /*  west        */
            case 'H':   run(4);         return(0);     /*  west        */
            case 'l':   moveplayer(2);  return(0);     /*  east        */
            case 'L':   run(2);         return(0);     /*  east        */
            case 'j':   moveplayer(1);  return(0);     /*  south       */
            case 'J':   run(1);         return(0);     /*  south       */
            case 'k':   moveplayer(3);  return(0);     /*  north       */
            case 'K':   run(3);         return(0);     /*  north       */
            case 'u':   moveplayer(5);  return(0);     /*  northeast   */
            case 'U':   run(5);         return(0);     /*  northeast   */
            case 'y':   moveplayer(6);  return(0);     /*  northwest   */
            case 'Y':   run(6);         return(0);     /*  northwest   */
            case 'n':   moveplayer(7);  return(0);     /*  southeast   */
            case 'N':   run(7);         return(0);     /*  southeast   */
            case 'b':   moveplayer(8);  return(0);     /*  southwest   */
            case 'B':   run(8);         return(0);     /*  southwest   */

            case '.':                               /*  stay here       */
                if (yrepcount) 
                    viewflag=1;
                return(0);

            case 'c':
                yrepcount=0;
                cast();
                return(0);     /*  cast a spell    */

            case 'd':
                yrepcount=0;
                if (c[TIMESTOP]==0)
                    dropobj();
                return(0); /*  to drop an object   */

            case 'e':
                yrepcount=0;
                if (c[TIMESTOP]==0)
                    if (!floor_consume( OCOOKIE, "eat" ))
                        consume( OCOOKIE, "eat", showeat );
                return(0); /*  to eat a fortune cookie */

            case 'g':   
                yrepcount = 0 ;
                cursors();
                lprintf("\nThe stuff you are carrying presently weighs %d pounds",(long)packweight());
                break ;

            case 'i':       /* inventory */
                yrepcount=0;
                nomove=1;
                showstr(FALSE);
                return(0);

            case 'p':           /* pray at an altar */
                yrepcount = 0;
                if (!prompt_mode)
                    pray_at_altar();
                else
                    nomove = 1;
                return(0);

            case 'q':           /* quaff a potion */
                yrepcount=0;
                if (c[TIMESTOP]==0)
                    if (!floor_consume( OPOTION, "quaff"))
                        consume( OPOTION, "quaff", showquaff );
                return(0);

            case 'r':
                yrepcount=0;
                if (c[BLINDCOUNT])
                    {
                    cursors();
                    lprcat("\nYou can't read anything when you're blind!");
                    }
                else if (c[TIMESTOP]==0)
                    if (!floor_consume( OSCROLL, "read" ))
                        if (!floor_consume( OBOOK, "read" ))
                            consume( OSCROLL, "read", showread );
                return(0);     /*  to read a scroll    */

            case 's':
                yrepcount = 0 ;
                if (!prompt_mode)
                    sit_on_throne();
                else
                    nomove = 1;
                return(0);

            case 't':                       /* Tidy up at fountain */
                yrepcount = 0 ;
                if (!prompt_mode)
                    wash_fountain() ;
                else
                    nomove = 1;
                return(0);

            case 'v':
                yrepcount=0;
                nomove = 1;
                cursors();
                lprintf("\nCaverns of Larn, Version %d.%d.%d, Diff=%d",(long)VERSION,(long)SUBVERSION,(long)PATCHLEVEL,(long)c[HARDGAME]);
                if (wizard)
                    lprcat(" Wizard");
                if (cheat) 
                    lprcat(" Cheater");
                lprcat("\nThis version of Larn by Gibbon");
                return(0);

            case 'w':                       /*  wield a weapon */
                yrepcount=0;
                wield();
                return(0);

            case 'A':
                yrepcount = 0;
                if (!prompt_mode)
                    desecrate_altar();
                else
                    nomove = 1;
                return(0);

            case 'C':                       /* Close something */
                yrepcount = 0 ;
                if (!prompt_mode)
                    close_something();
                else
                    nomove = 1;
                return(0);

            case 'D':                       /* Drink at fountain */
                yrepcount = 0 ;
                if (!prompt_mode)
                    drink_fountain() ;
                else
                    nomove = 1;
                return(0);

            case 'E':               /* Enter a building */
                yrepcount = 0 ;
                if (!prompt_mode)
                    enter() ;
                else
                    nomove = 1;
                break ;

            case 'I':              /*  list spells and scrolls */
                yrepcount=0;
                seemagic(0);
                nomove=1;
                return(0);

            case 'O':               /* Open something */
                yrepcount = 0 ;
                if (!prompt_mode)
                    open_something();
                else
                    nomove = 1;
                return(0);

            case 'P':
                cursors();
                yrepcount = 0;
                nomove = 1;
                if (outstanding_taxes>0)
                    lprintf("\nYou presently owe %d gp in taxes.",(long)outstanding_taxes);
                else
                    lprcat("\nYou do not owe any taxes.");
                return(0);

            case 'Q':    /*  quit        */
                yrepcount=0;
                quit();
                nomove=1;
                return(0);

            case 'R' :          /* remove gems from a throne */
                yrepcount = 0 ;
                if (!prompt_mode)
                    remove_gems( );
                else
                    nomove = 1;
                return(0);

        case 'S':
            clear();
            lprcat("Saving . . .");
            lflush();
            savegame(savefilename);
            wizard = 1;
            died(-257); /* save the game - doesn't return    */
            break;

            case 'T':   yrepcount=0;    cursors();  if (c[SHIELD] != -1) { c[SHIELD] = -1; lprcat("\nYour shield is off"); bottomline(); } else
                                        if (c[WEAR] != -1) { c[WEAR] = -1; lprcat("\nYour armor is off"); bottomline(); }
                        else lprcat("\nYou aren't wearing anything");
                        return(0);

            case 'W':
                yrepcount=0;
                wear();
                return(0); /*  wear armor  */

            case 'Z':
                yrepcount=0;
                if (c[LEVEL]>9) 
                    { 
                    oteleport(1);
                    return(0);
                    }
                cursors(); 
                lprcat("\nAs yet, you don't have enough experience to use teleportation");
                return(0); /*  teleport yourself   */

            case ' ':   yrepcount=0;    nomove=1;  return(0);

# ifdef MSDOS
            case 'D'-64:
                yrepcount = 0;
                nomove = 1;
                levelinfo();
                return(0);
# endif

            case 'L'-64:  yrepcount=0;  drawscreen();  nomove=1; return(0);    /*  look        */

#if WIZID
#ifdef EXTRA
            case 'A'-64:    yrepcount=0;    nomove=1; if (wizard) { diag(); return(0); }  /*   create diagnostic file */
                        return(0);
#endif
#endif
            case '<':                       /* Go up stairs or vol shaft */
                yrepcount = 0 ;
                if (!prompt_mode)
                    up_stairs();
        else
            nomove = 1;
                return(0);

            case '>':                       /* Go down stairs or vol shaft*/
                yrepcount = 0 ;
                if (!prompt_mode)
                    down_stairs();
        else
            nomove = 1;
                return(0);

            case '?':                       /* give the help screen */
                yrepcount=0;
                help();
                nomove=1;
                return(0);

        case ',':                       /* pick up an item */
            yrepcount = 0 ;
            if (!prompt_mode)
            /* pickup, don't identify or prompt for action */
            lookforobject( FALSE, TRUE, FALSE );
        else
            nomove = 1;
        return(0);

            case ':':                       /* look at object */
                yrepcount = 0 ;
                if (!prompt_mode)
            /* identify, don't pick up or prompt for action */
                    lookforobject( TRUE, FALSE, FALSE );
                nomove = 1;  /* assumes look takes no time */
                return(0);

        case '@':       /* toggle auto-pickup */
            yrepcount = 0 ;
            nomove = 1;
            cursors();
            lprcat("\nAuto pickup: ");
            auto_pickup = !auto_pickup;
            if (auto_pickup)
                lprcat("On.");
            else
                lprcat("Off.");
            return(0);

        case '/':        /* identify object/monster */
            specify_object();
            nomove = 1 ;
            yrepcount = 0 ;
            return(0);

        case '^':                       /* identify traps */
                flag = yrepcount = 0;
                cursors();
                lprc('\n');
                for (j=playery-1; j<playery+2; j++)
                    {
                    if (j < 0)
                        j=0;
                    if (j >= MAXY)
                        break;
                    for (i=playerx-1; i<playerx+2; i++)
                        {
                        if (i < 0) 
                            i=0;
                        if (i >= MAXX) 
                            break;
                        switch(item[i][j])
                            {
                            case OTRAPDOOR:     case ODARTRAP:
                            case OTRAPARROW:    case OTELEPORTER:
                            case OPIT:
                                lprcat("\nIts ");
                                lprcat(objectname[item[i][j]]);
                                flag++;
                            };
                        }
                    }
                if (flag==0) 
                    lprcat("\nNo traps are visible");
                return(0);

#if WIZID
            case '_':   /*  this is the fudge player password for wizard mode*/
                        yrepcount=0;    cursors(); nomove=1;

                        if (getpassword()==0)
                            {
                            scbr(); /* system("stty -echo cbreak"); */
                                return(0);
                            }
                        wizard=1;  scbr(); /* system("stty -echo cbreak"); */
                        for (i=0; i<6; i++)  c[i]=70;  iven[0]=iven[1]=0;
                        take(OPROTRING,50);   take(OLANCE,25);  c[WIELD]=1;
                        c[LANCEDEATH]=1;   c[WEAR] = c[SHIELD] = -1;
                        raiseexperience(6000000L);  c[AWARENESS] += 25000;
                        {
                int i,j;
                for (i = 0; i < MAXY; i++)
                    for (j = 0; j < MAXX; j++)
                        know[j][i] = 1;
                for (i = 0; i < SPNUM; i++)
                    spelknow[i] = 1;
                for (i = 0; i < MAXSCROLL; i++)
                    scrollname[i] = scrollname[i];
                for (i = 0; i < MAXPOTION; i++)
                    potionname[i] = potionname[i];
                        }
            for (i = 0; i < MAXSCROLL; i++)
                if (strlen(scrollname[i]) > 2) {    /* no null items */
                    item[i][0] = OSCROLL;
                    iarg[i][0] = i;
                }
            for (i = MAXX - 1; i > MAXX - 1 - MAXPOTION; i--)
                if (strlen(potionname[i - MAXX + MAXPOTION]) > 2) { /* no null items */
                    item[i][0] = OPOTION;
                    iarg[i][0] = i - MAXX + MAXPOTION;
                }
            for (i = 1; i < MAXY; i++) {
                item[0][i] = i;
                iarg[0][i] = 0;
            }
            for (i = MAXY; i < MAXY + MAXX; i++) {
                item[i - MAXY][MAXY - 1] = i;
                iarg[i - MAXY][MAXY - 1] = 0;
            }
            for (i = MAXX + MAXY; i < MAXX + MAXY + MAXY; i++) {
                item[MAXX - 1][i - MAXX - MAXY] = i;
                iarg[MAXX - 1][i - MAXX - MAXY] = 0;
            }
            c[GOLD] += 25000;
            drawscreen();
            return(0);
#endif

            };
        }
    }

parse2()
    {
    if (c[HASTEMONST]) movemonst(); movemonst(); /* move the monsters       */
    randmonst();    regen();
    }

run(dir)
    int dir;
    {
    register int i;
    i=1; while (i)
        {
        i=moveplayer(dir);
        if (i>0) {  if (c[HASTEMONST]) movemonst();  movemonst(); randmonst(); regen(); }
        if (hitflag) i=0;
        if (i!=0)  showcell(playerx,playery);
        }
    }

/*
    function to wield a weapon
 */
wield()
    {
    register int i;
    while (1)
        {
        if ((i = whatitem("wield (- for nothing)")) == '\33')
        return(0);
        if (i != '.')
            {
            if (i=='*')
                {
                i = showwield();
                cursors();
                }
            if ( i == '-' )
                {
                c[WIELD] = -1 ;
                bottomline();
                return(0);
                }
            if (i && i != '.')
                if (iven[i-'a']==0)
                    { ydhi(i); return(0); }
                else if (iven[i-'a']==OPOTION)
                    { ycwi(i); return(0); }
            else if (iven[i-'a']==OSCROLL)
                    { ycwi(i); return(0); }
            else if ((c[SHIELD]!= -1) && (iven[i-'a']==O2SWORD))
                    { lprcat("\nBut one arm is busy with your shield!");
                      return(0); }
            else
                {
                c[WIELD]=i-'a';
                if (iven[i-'a'] == OLANCE)
                    c[LANCEDEATH]=1;
                else c[LANCEDEATH]=0;
                bottomline();
                return(0);
                }
            }
        }
    }

/*
    common routine to say you don't have an item
 */
ydhi(x)
    int x;
    { cursors();  lprintf("\nYou don't have item %c!",x); }
ycwi(x)
    int x;
    { cursors();  lprintf("\nYou can't wield item %c!",x); }

/*
    function to wear armor
 */
wear()
    {
    register int i;
    while (1)
        {
        if ((i = whatitem("wear"))=='\33')
            return(0);
        if (i != '.' && i != '-')
            {
            if (i=='*')
                {
                i = showwear();
                cursors();
                }
            if (i && i != '.')
                switch(iven[i-'a'])
                    {
                    case 0:
                        ydhi(i);
                        return(0);
                    case OLEATHER:  case OCHAIN:  case OPLATE:
                    case ORING:     case OSPLINT: case OPLATEARMOR:
                    case OSTUDLEATHER:            case OSSPLATE:
                        if (c[WEAR] != -1) { lprcat("\nYou're already wearing some armor"); return(0); }
                            c[WEAR]=i-'a';  bottomline(); return(0);
                    case OSHIELD:   if (c[SHIELD] != -1) { lprcat("\nYou are already wearing a shield"); return(0); }
                                if (iven[c[WIELD]]==O2SWORD) { lprcat("\nYour hands are busy with the two handed sword!"); return(0); }
                                c[SHIELD] = i-'a';  bottomline(); return(0);
                    default:    lprcat("\nYou can't wear that!");
                    };
            }
        }
    }

/*
    function to drop an object
 */
dropobj()
    {
    register int i;
    register char *p;
    long amt;

    p = &item[playerx][playery];
    while (1)
        {
        if ((i = whatitem("drop"))=='\33')  
        return(0);
    if (i=='*') 
        {
        i = showstr(TRUE);
        cursors();
        }
    if ( i != '-' )
            {
            if (i=='.') /* drop some gold */
                {
                if (*p) { lprcat("\nThere's something here already!"); return(0); }
                lprcat("\n\n");
                cl_dn(1,23);
                lprcat("How much gold do you drop? ");
                if ((amt=readnum((long)c[GOLD])) == 0) return(0);
                if (amt>c[GOLD])
                    {
#ifndef MSDOS
            lprcat("\n");
#endif MSDOS
            lprcat("You don't have that much!");
            return(0); }
                if (amt<=32767)
                    { *p=OGOLDPILE; i=amt; }
                else if (amt<=327670L)
                    { *p=ODGOLD; i=amt/10; amt = 10L*i; }
                else if (amt<=3276700L)
                    { *p=OMAXGOLD; i=amt/100; amt = 100L*i; }
                else if (amt<=32767000L)
                    { *p=OKGOLD; i=amt/1000; amt = 1000L*i; }
                else
                    { *p=OKGOLD; i=32767; amt = 32767000L; }
                c[GOLD] -= amt;
#ifndef MSDOS
                lprintf("You drop %d gold pieces",(long)amt);
#else
                lprintf("\nYou drop %d gold pieces",(long)amt);
#endif MSDOS
                iarg[playerx][playery]=i; bottomgold();
                know[playerx][playery]=0; dropflag=1;  return(0);
                }
        if (i)
        {
        drop_object(i-'a');
        return(0);
        }
            }
        }
    }

int floor_consume( search_item, cons_verb )
int search_item;
char *cons_verb;
    {
    register int i;
    char tempc;

    cursors();
    i = item[playerx][playery];

    /* item not there, quit
    */
    if (i != search_item)
        return( 0 );

    /* item there.  does the player want to consume it?
    */
    lprintf("\nThere is %s", objectname[i] );
    if (i==OSCROLL)
        if (scrollname[iarg[playerx][playery]][0])
            lprintf(" of%s", scrollname[iarg[playerx][playery]]);
    if (i==OPOTION)
        if (potionname[iarg[playerx][playery]][0])
            lprintf(" of%s", potionname[iarg[playerx][playery]]);
    lprintf(" here.  Do you want to %s it?", cons_verb );

    if ((tempc = getyn()) == 'n' )
        return( 0 );                /* item there, not consumed */
    else if (tempc != 'y')
        {
        lprcat(" aborted");
        return( -1 );               /* abort */
        }

    /* consume the item.
    */
    switch( i )
        {
        case OCOOKIE:
            outfortune();
            forget();
            break;
        case OBOOK:
            readbook( iarg[playerx][playery] );
            forget();
            break;
        case OPOTION:
            quaffpotion( iarg[playerx][playery] );
            forget();
            break;
        case OSCROLL:
            /* scrolls are tricky because of teleport.
            */
            i = iarg[playerx][playery];
            know[playerx][playery] = 0;
            item[playerx][playery] = iarg[playerx][playery] = 0 ;
            read_scroll( i );
            break;
        }
    return( 1 );
    }

int consume( search_item, prompt, showfunc )
int search_item ;
char *prompt;
int (*showfunc)();
    {
    register int i;

    while (1)
        {
        if ((i = whatitem( prompt )) == '\33')
            return(0);
        if (i != '.' && i != '-')
            {
            if (i == '*')
                {
                i = showfunc();
                cursors();
                }
            if (i && i != '.')
                {
                switch (iven[i-'a'])
                    {
                    case OSCROLL:
                        if ( search_item != OSCROLL )
                            {
                            lprintf("\nYou can't %s that.", prompt );
                            return(0);
                            }
                        read_scroll( ivenarg[i-'a'] );
                        break;
                    case OBOOK:
                        if ( search_item != OSCROLL )
                            {
                            lprintf("\nYou can't %s that.", prompt );
                            return(0);
                            }
                        readbook( ivenarg[i-'a'] );
                        break;
                    case OCOOKIE:
                        if ( search_item != OCOOKIE )
                            {
                            lprintf("\nYou can't %s that.", prompt );
                            return(0);
                            }
                        outfortune();
                        break;
                    case OPOTION:
                        if ( search_item != OPOTION )
                            {
                            lprintf("\nYou can't %s that.", prompt );
                            return(0);
                            }
                        quaffpotion( ivenarg[i-'a'], TRUE );
                        break;
                    case 0:
                        ydhi(i);
                        return(0);
                    default:
                        lprintf("\nYou can't %s that.", prompt );
                        return(0);
                    }
                iven[i-'a'] = 0;
                return(0);
                }
            }
        }
    }

/*
    function to ask what player wants to do
 */
whatitem(str)
    char *str;
    {
    int i=0;
    cursors();  lprintf("\nWhat do you want to %s [* for all] ? ",str);
    while (i>'z' || (i<'a' && i!='-' && i!='*' && i!='\33' && i!='.'))
        i=ttgetch();
    if (i=='\33')
        lprcat(" aborted");
    return(i);
    }

/*
    subroutine to get a number from the player
    and allow * to mean return amt, else return the number entered
 */
unsigned long readnum(mx)
    long mx;
    {
    register int i;
    register unsigned long amt=0;

    sncbr();
    /* allow him to say * for all gold 
    */
    if ((i=ttgetch()) == '*')
        amt = mx;
    else
        /* read chars into buffer, deleting when requested */
    while (i != '\n')
        {
        if (i=='\033') { scbr(); lprcat(" aborted"); return(0); }
        if ((i <= '9') && (i >= '0') && (amt<999999999))
            amt = amt*10+i-'0';
        if ((i=='\010') || (i=='\177'))
            amt = (long)(amt / 10) ;
        i = ttgetch();
        }
    scbr();
    return(amt);
    }

#ifdef HIDEBYLINK
/*
 *  routine to zero every byte in a string
 */
szero(str)
    register char *str;
    {
    while (*str)
        *str++ = 0;
    }
#endif HIDEBYLINK

#ifdef TIMECHECK
/*
 *  routine to check the time of day and return 1 if its during work hours
 *  checks the file ".holidays" for forms like "mmm dd comment..."
 */
int playable()
    {
    long g_time,time();
    int hour,day,year;
    char *date,*month,*p;

    time(&g_time);  /* get the time and date */
    date = ctime(&g_time); /* format: Fri Jul  4 00:27:56 EDT 1986 */
    year = atoi(date+20);
    hour = (date[11]-'0')*10 + date[12]-'0';
    day  = (date[8]!=' ') ? ((date[8]-'0')*10 + date[9]-'0') : (date[9]-'0');
    month = date+4;  date[7]=0; /* point to and NULL terminate month */

    if (((hour>=8 && hour<17)) /* 8AM - 5PM */
        && strncmp("Sat",date,3)!=0     /* not a Saturday */
        && strncmp("Sun",date,3)!=0)    /* not a Sunday */
            {
        /* now check for a .holidays datafile */
            lflush();
            if (lopen(holifile) >= 0)
                for ( ; ; )
                    {
                    if ((p=lgetw())==0) break;
                    if (strlen(p)<6) continue;
                    if ((strncmp(p,month,3)==0) && (day==atoi(p+4)) && (year==atoi(p+7)))
                        return(0); /* a holiday */
                    }
            lrclose();  lcreat((char*)0);
            return(1);
            }
    return(0);
    }
#endif TIMECHECK
