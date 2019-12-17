/* savelev.c */
/* Removed MSDOS stuff ~Gibbon */
#include "savelev.h"
#include "header.h"
#include "larndefs.h"

/*
 *  routine to save the present level into storage
 */
void
savelevel(void)
    {
    register struct cel *pcel;
    register char *pitem,*pknow,*pmitem;
    register short *phitp,*piarg;
    register struct cel *pecel;

    pcel = &cell[level*MAXX*MAXY];  /* pointer to this level's cells */
    pecel = pcel + MAXX*MAXY;   /* pointer to past end of this level's cells */
    pitem=item[0]; piarg=iarg[0]; pknow=know[0]; pmitem=mitem[0]; phitp=hitp[0];
    while (pcel < pecel)
        {
        pcel->mitem  = *pmitem++;
        pcel->hitp   = *phitp++;
        pcel->item   = *pitem++;
        pcel->know   = *pknow++;
        pcel->iarg   = *piarg++;
        pcel++;
        }
    }

/*
 *  routine to restore a level from storage
 */
void
getlevel(void)
    {
    register struct cel *pcel;
    register char *pitem,*pknow,*pmitem;
    register short *phitp,*piarg;
    register struct cel *pecel;

    pcel = &cell[level*MAXX*MAXY];  /* pointer to this level's cells */
    pecel = pcel + MAXX*MAXY;   /* pointer to past end of this level's cells */
    pitem=item[0]; piarg=iarg[0]; pknow=know[0]; pmitem=mitem[0]; phitp=hitp[0];
    while (pcel < pecel)
        {
        *pmitem++ = pcel->mitem;
        *phitp++ = pcel->hitp;
        *pitem++ = pcel->item;
        *pknow++ = pcel->know;
        *piarg++ = pcel->iarg;
        pcel++;
        }
    }
