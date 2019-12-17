/*  global.c
 *
 *  raiselevel()        subroutine to raise the player one level
 *  loselevel()         subroutine to lower the player by one level
 *  raiseexperience(x)  subroutine to increase experience points
 *  loseexperience(x)   subroutine to lose experience points
 *  losehp(x)           subroutine to remove hit points from the player
 *  losemhp(x)          subroutine to remove max # hit points from the player
 *  raisehp(x)          subroutine to gain hit points
 *  raisemhp(x)         subroutine to gain maximum hit points
 *  losemspells(x)      subroutine to lose maximum spells
 *  raisemspells(x)     subroutine to gain maximum spells
 *  makemonst(lev)      function to return monster number for a randomly selected monster
 *  positionplayer()    function to be sure player is not in a wall
 *  recalc()            function to recalculate the armor class of the player
 *  quit()              subroutine to ask if the player really wants to quit
 *  more()
 *  take()
 *  drop_object()
 *  enchantarmor()
 *  enchweapon()
 *  pocketfull()
 *  nearbymonst()
 *  stealsomething()
 *  emptyhanded()
 *  creategem()
 *  adjustcvalues()
 *  gettokstr()
 *  getpassword()
 *  getyn()
 *  packweight()
 */

#include <ctype.h>
#include "global.h"
#include "header.h"
#include "larndefs.h"
#include "monsters.h"
#include "objects.h"
#include "player.h"

extern int score[],dropflag;
extern short playerx,playery,lastnum;
extern char cheat,level,monstnamelist[];
extern char lastmonst[],*what[],*who[]; 
extern char winner[];
extern char logname[],monstlevel[];
extern char sciv[SCORESIZE+1][26][2],*potionname[],*scrollname[];

#ifdef __STDC__
extern void show3( int );
#else
extern void show3();
#endif

/*
    raiselevel()

    subroutine to raise the player one level
    uses the skill[] array to find level boundarys
    uses c[EXPERIENCE]  c[LEVEL]
 */
raiselevel()
    {
    if (c[LEVEL] < MAXPLEVEL) raiseexperience((long)(skill[c[LEVEL]]-c[EXPERIENCE]));
    }

/*
    loselevel()

    subroutine to lower the players character level by one
 */
loselevel()
    {
    if (c[LEVEL] > 1) loseexperience((long)(c[EXPERIENCE] - skill[c[LEVEL]-1] + 1));
    }

/*
    raiseexperience(x)

    subroutine to increase experience points
 */
raiseexperience(x)
    register long x;
    {
    register int i,tmp;
    i=c[LEVEL]; c[EXPERIENCE]+=x;
    while (c[EXPERIENCE] >= skill[c[LEVEL]] && (c[LEVEL] < MAXPLEVEL))
        {
        tmp = (c[CONSTITUTION]-c[HARDGAME])>>1;
        c[LEVEL]++; raisemhp((int)(rnd(3)+rnd((tmp>0)?tmp:1)));
        raisemspells((int)rund(3));
        if (c[LEVEL] < 7-c[HARDGAME]) raisemhp((int)(c[CONSTITUTION]>>2));
        }
    if (c[LEVEL] != i)
        {
        cursors();
        beep(); lprintf("\nWelcome to level %d",(long)c[LEVEL]);    /* if we changed levels */
        }
    bottomline();
    }

/*
    loseexperience(x)

    subroutine to lose experience points
 */
loseexperience(x)
    register long x;
    {
    register int i,tmp;
    i=c[LEVEL];     c[EXPERIENCE]-=x;
    if (c[EXPERIENCE] < 0) c[EXPERIENCE]=0;
    while (c[EXPERIENCE] < skill[c[LEVEL]-1])
        {
        if (--c[LEVEL] <= 1) c[LEVEL]=1;    /*  down one level      */
        tmp = (c[CONSTITUTION]-c[HARDGAME])>>1; /* lose hpoints */
        losemhp((int)rnd((tmp>0)?tmp:1));   /* lose hpoints */
        if (c[LEVEL] < 7-c[HARDGAME]) losemhp((int)(c[CONSTITUTION]>>2));
        losemspells((int)rund(3));              /*  lose spells     */
        }
    if (i!=c[LEVEL])
        {
        cursors();
        beep(); lprintf("\nYou went down to level %d!",(long)c[LEVEL]);
        }
    bottomline();
    }

/*
    losehp(x)
    losemhp(x)

    subroutine to remove hit points from the player
    warning -- will kill player if hp goes to zero
 */
losehp(x)
    register int x;
    {
    if ((c[HP] -= x) <= 0)
        {
        beep(); lprcat("\n");  nap(3000);  died(lastnum);
        }
    }

losemhp(x)
    register int x;
    {
    c[HP] -= x;     if (c[HP] < 1)      c[HP]=1;
    c[HPMAX] -= x;  if (c[HPMAX] < 1)   c[HPMAX]=1;
    }

/*
    raisehp(x)
    raisemhp(x)

    subroutine to gain maximum hit points
 */
raisehp(x)
    register int x;
    {
    if ((c[HP] += x) > c[HPMAX]) c[HP] = c[HPMAX];
    }

raisemhp(x)
    register int x;
    {
    c[HPMAX] += x;  c[HP] += x;
    }

/*
    raisemspells(x)

    subroutine to gain maximum spells
*/
raisemspells(x)
    register int x;
    {
    c[SPELLMAX]+=x; c[SPELLS]+=x;
    }

/*
    losemspells(x)

    subroutine to lose maximum spells
*/
losemspells(x)
    register int x;
    {
    if ((c[SPELLMAX] -= x) < 0) c[SPELLMAX]=0;
    if ((c[SPELLS] -= x) < 0) c[SPELLS]=0;
    }

/*
    makemonst(lev)
        int lev;

    function to return monster number for a randomly selected monster
        for the given cave level    
 */
makemonst(lev)
    register int lev;
    {
    register int tmp,x;
    if (lev < 1)
    lev = 1;
    if (lev > 12)
    lev = 12;
    if (lev < 5)
    tmp=rnd((x=monstlevel[lev-1])?x:1);
    else
        tmp=rnd((x=monstlevel[lev-1]-monstlevel[lev-4])?x:1)+monstlevel[lev-4];

    while (monster[tmp].genocided && tmp<MAXMONST)
    tmp++; /* genocided? */
    return(tmp);
    }

/*
    positionplayer()

    Insure player is not in a wall or on top of a monster.  Could be more
    intelligent about what kinds of objects the player can land on.
 */
positionplayer()
    {
    int z, try = 2;

    /* set the previous player x,y position to the new one, so that
       clearing the player indicator from the previous location will
       not do the wrong thing.
    */
    oldx = playerx ;
    oldy = playery ;

    /* short-circuit the testing if current position empty
    */
    if (!item[playerx][playery] &&
        !mitem[playerx][playery])
        return(0);

    /* make at most two complete passes across the dungeon, looking
       for a clear space.  In most situations, should find a clear
       spot right around the current player position.
    */
    do
        {

        /* check all around the player position for a clear space.
        */
        for (z=1; z<9 ; z++)
            {
            int tmpx = playerx + diroffx[z];
            int tmpy = playery + diroffy[z];
            if (!item[tmpx][tmpy] &&
                !mitem[tmpx][tmpy])
                {
                playerx = tmpx ;
                playery = tmpy ;
                return(0);
                }
            }

        /* no clear spots around the player. try another position,
           wrapping around the dungeon.
        */
        if (++playerx >= MAXX-1)
            {
            playerx = 1;
            if (++playery >= MAXY-1)
                {
                playery = 1;
                try--;
                }
            }
        }
    while (try);

    /* no spot found.
    */
    lprcat("Failure in positionplayer\n");
    }

/*
    recalc()    function to recalculate the armor class of the player
 */
recalc()
    {
    register int i,j,k;
    c[AC] = c[MOREDEFENSES];
    if (c[WEAR] >= 0)  
        switch(iven[c[WEAR]])
            {
            case OSHIELD:       c[AC] += 2 + ivenarg[c[WEAR]]; break;
            case OLEATHER:      c[AC] += 2 + ivenarg[c[WEAR]]; break;
            case OSTUDLEATHER:  c[AC] += 3 + ivenarg[c[WEAR]]; break;
            case ORING:         c[AC] += 5 + ivenarg[c[WEAR]]; break;
            case OCHAIN:        c[AC] += 6 + ivenarg[c[WEAR]]; break;
            case OSPLINT:       c[AC] += 7 + ivenarg[c[WEAR]]; break;
            case OPLATE:        c[AC] += 9 + ivenarg[c[WEAR]]; break;
            case OPLATEARMOR:   c[AC] += 10 + ivenarg[c[WEAR]]; break;
            case OSSPLATE:      c[AC] += 12 + ivenarg[c[WEAR]]; break;
            }

    if (c[SHIELD] >= 0) if (iven[c[SHIELD]] == OSHIELD) c[AC] += 2 + ivenarg[c[SHIELD]];
    if (c[WIELD] < 0)  c[WCLASS] = 0;  else
        {
        i = ivenarg[c[WIELD]];
        switch(iven[c[WIELD]])
            {
            case ODAGGER:    c[WCLASS] =  3 + i;  break;
            case OBELT:      c[WCLASS] =  7 + i;  break;
            case OSHIELD:    c[WCLASS] =  8 + i;  break;
            case OSPEAR:     c[WCLASS] = 10 + i;  break;
            case OFLAIL:     c[WCLASS] = 14 + i;  break;
            case OBATTLEAXE: c[WCLASS] = 17 + i;  break;
            case OLANCE:     c[WCLASS] = 19 + i;  break;
            case OLONGSWORD: c[WCLASS] = 22 + i;  break;
            case O2SWORD:    c[WCLASS] = 26 + i;  break;
            case OSWORD:     c[WCLASS] = 32 + i;  break;
            case OSWORDofSLASHING: c[WCLASS] = 30 + i; break;
            case OHAMMER:    c[WCLASS] = 35 + i;  break;
            default:         c[WCLASS] = 0;
            }
        }
    c[WCLASS] += c[MOREDAM];

/*  now for regeneration abilities based on rings   */
    c[REGEN]=1;     c[ENERGY]=0;
    j=0;  for (k=25; k>0; k--)  if (iven[k]) {j=k; k=0; }
    for (i=0; i<=j; i++)
        {
        switch(iven[i])
            {
            case OPROTRING: c[AC]     += ivenarg[i] + 1;    break;
            case ODAMRING:  c[WCLASS] += ivenarg[i] + 1;    break;
            case OBELT:     c[WCLASS] += ((ivenarg[i]<<1)) + 2; break;

            case OREGENRING:    c[REGEN]  += ivenarg[i] + 1;    break;
            case ORINGOFEXTRA:  c[REGEN]  += 5 * (ivenarg[i]+1); break;
            case OENERGYRING:   c[ENERGY] += ivenarg[i] + 1;    break;
            }
        }
    }


/*
    quit()

    subroutine to ask if the player really wants to quit
 */
quit()
    {
    register int i;
    cursors();  strcpy(lastmonst,"");
    lprcat("\n\nDo you really want to quit?");
    while (1)
        {
        i=ttgetch();
        if ((i == 'y') || (i == 'Y'))   
            { 
            died(300); 
            return(0);
            }
        if ((i == 'n') || (i == 'N') || (i == '\33')) 
            { 
            lprcat(" no"); 
            lflush(); 
            return(0);
            }
        lprcat("\n");  
        setbold();  lprcat("Yes");  resetbold();  
        lprcat(" or ");
        setbold();  lprcat("No");   resetbold();  
        lprcat(" please?   Do you want to quit? ");
        }   
    }

/*
    function to ask --more--. If the user enters a space, returns 0.  If user
    enters Escape, returns 1.  If user enters alphabetic, then returns that
    value.
 */
more(select_allowed)
char select_allowed;
    {
    register int i;

    lprcat("\n  --- press ");  standout("space");  lprcat(" to continue --- ");
    if (select_allowed)
        lprcat("letter to select --- ");

    while (TRUE)
        {
        if ((i=ttgetch()) == ' ' || i == '\n')
            return 0 ;
        if (i== '\x1B')
            return 1 ;
	if (select_allowed)
	    {
	    if (isupper(i))
		i = tolower(i);
	    if ( i >= 'a' && i <= 'z' || i == '.' )
		return i;
	    }
        }
    }

/*
    function to enchant armor player is currently wearing
 */
enchantarmor()
    {
    register int tmp;
    if (c[WEAR]<0) { if (c[SHIELD] < 0)
        { cursors(); beep(); lprcat("\nYou feel a sense of loss"); return(0); }
                    else { tmp=iven[c[SHIELD]]; if (tmp != OSCROLL) if (tmp != OPOTION) { ivenarg[c[SHIELD]]++; bottomline(); } } }
    tmp = iven[c[WEAR]];
    if (tmp!=OSCROLL) if (tmp!=OPOTION)  { ivenarg[c[WEAR]]++;  bottomline(); }
    }

/*
    function to enchant a weapon presently being wielded
 */
enchweapon()
    {
    register int tmp;
    if (c[WIELD]<0)
        { cursors(); beep(); lprcat("\nYou feel a sense of loss"); return(0); }
    tmp = iven[c[WIELD]];
    if (tmp!=OSCROLL) if (tmp!=OPOTION)
        { ivenarg[c[WIELD]]++;
          if (tmp==OCLEVERRING) c[INTELLIGENCE]++;  else
          if (tmp==OSTRRING)    c[STREXTRA]++;  else
          if (tmp==ODEXRING)    c[DEXTERITY]++;       bottomline(); }
    }

/*
    function to return 1 if a monster is next to the player else returns 0
 */
nearbymonst()
    {
    register int tmp,tmp2;
    for (tmp=playerx-1; tmp<playerx+2; tmp++)
        for (tmp2=playery-1; tmp2<playery+2; tmp2++)
            if (mitem[tmp][tmp2]) return(1); /* if monster nearby */
    return(0);
    }

/*
    function to steal an item from the players pockets
    returns 1 if steals something else returns 0
 */
stealsomething()
    {
    register int i,j;
    j=100;
    while (1)
        {
        i=rund(26);
        if (iven[i]) if (c[WEAR]!=i) if (c[WIELD]!=i) if (c[SHIELD]!=i)
            {
            show3(i);
            adjustcvalues(iven[i],ivenarg[i]);  iven[i]=0; return(1);
            }
        if (--j <= 0) return(0);
        }
    }

/*
    function to return 1 is player carrys nothing else return 0
 */
emptyhanded()
    {
    register int i;
    for (i=0; i<26; i++)
        if (iven[i]) if (i!=c[WIELD]) if (i!=c[WEAR]) if (i!=c[SHIELD]) return(0);
    return(1);
    }

/*
    function to create a gem on a square near the player
 */
creategem()
    {
    register int i,j;
    switch(rnd(4))
        {
        case 1:  i=ODIAMOND;    j=50;   break;
        case 2:  i=ORUBY;       j=40;   break;
        case 3:  i=OEMERALD;    j=30;   break;
        default: i=OSAPPHIRE;   j=20;   break;
        };
    createitem(i,rnd(j)+j/10);
    }

/*
    function to change character levels as needed when dropping an object
    that affects these characteristics
 */
adjustcvalues(itm,arg)
    int itm,arg;
    {
    register int flag;
    flag=0;
    switch(itm)
        {
        case ODEXRING:  c[DEXTERITY] -= arg+1;  flag=1; break;
        case OSTRRING:  c[STREXTRA]  -= arg+1;  flag=1; break;
        case OCLEVERRING: c[INTELLIGENCE] -= arg+1;  flag=1; break;
        case OHAMMER:   c[DEXTERITY] -= 10; c[STREXTRA] -= 10;
                        c[INTELLIGENCE] += 10; flag=1; break;
        case OSWORDofSLASHING:  c[DEXTERITY] -= 5;  flag=1; break;
        case OORBOFDRAGON:      --c[SLAYING];       return(0);
        case OSPIRITSCARAB:     --c[NEGATESPIRIT];  return(0);
        case OCUBEofUNDEAD:     --c[CUBEofUNDEAD];  return(0);
        case ONOTHEFT:          --c[NOTHEFT];       return(0);
        case OLANCE:        c[LANCEDEATH]=0;    return(0);
        case OPOTION:   case OSCROLL:   return(0);

        default:    flag=1;
        };
    if (flag) bottomline();
    }

/*
    function to ask user for a password (no echo)
    returns 1 if entered correctly, 0 if not
 */
static char gpwbuf[33];
getpassword()
    {
    register int i,j;
    register char *gpwp;
    extern char *password;
    scbr(); /*  system("stty -echo cbreak"); */
    gpwp = gpwbuf;  lprcat("\nEnter Password: "); lflush();
    i = strlen(password);
    for (j=0; j<i; j++) 
        *gpwp++ = ttgetch();
    gpwbuf[i]=0;
    sncbr(); /* system("stty echo -cbreak"); */
    if (strcmp(gpwbuf,password) != 0)
        {   lprcat("\nSorry\n");  lflush(); return(0);  }
    else  return(1);
    }

/*
    subroutine to get a yes or no response from the user
    returns y or n
 */
getyn()
    {
    register int i=0;
    while (i!='y' && i!='n' && i!='\33')
    i=ttgetch();
    return(i);
    }

/*
    function to calculate the pack weight of the player
    returns the number of pounds the player is carrying
 */
packweight()
    {
    register int i, j=25, k;

    k=c[GOLD]/1000;
    while ((iven[j]==0) && (j>0))
    --j;
    for (i=0; i<=j; i++)
        switch(iven[i])
            {
        case 0:
        break;
        case OSSPLATE:
        case OPLATEARMOR:
        k += 40;
        break;
        case OPLATE:
        k += 35;
        break;
        case OHAMMER:
        k += 30;
        break;
        case OSPLINT:
        k += 26;
        break;
        case OSWORDofSLASHING:
        case OCHAIN:
        case OBATTLEAXE:
        case O2SWORD:
        k += 23;
        break;
        case OLONGSWORD:
        case OSWORD:
        case ORING:
        case OFLAIL:
        k += 20;
        break;
        case OLANCE:
        case OSTUDLEATHER:
        k += 15;
        break;
        case OLEATHER:
        case OSPEAR:
        k += 8;
        break;
        case OORBOFDRAGON:
        case OBELT:
        k += 4;
        break;
        case OSHIELD:
        k += 7;
        break;
        case OCHEST:
        k += 30 + ivenarg[i];
        break;
        default:
        k++;
        break;
            };
    return(k);
    }

/* generates random numbers 1<=rnd(N)<=N */
int rnd(int x)
{
	return ((((lrandx = lrandx * 1103515245 + 12345) >> 7) % (x)) + 1);
}

/* generates random numbers 0<=rund(N)<=N-1 */
int rund(int x)
{
	return ((((lrandx = lrandx * 1103515245 + 12345) >> 7) % (x)));
}

#if 0
/*
    function to read a string from token input "string"
    returns a pointer to the string
 */
gettokstr(str)
    register char *str;
    {
    register int i,j;
    i=50;
    while ((ttgetch() != '"') && (--i > 0));
    i=36;
    while (--i > 0)
        {
        if ((j=ttgetch()) != '"') *str++ = j;  else i=0;
        }
    *str = 0;
    i=50;
    if (j != '"') while ((ttgetch() != '"') && (--i > 0)); /* if end due to too long, then find closing quote */
    }
#endif
