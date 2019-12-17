/* help.c */
/* Removed MSDOS and VT100 stuff ~Gibbon */
#include "header.h"
#include "larndefs.h"

/*
 *  help function to display the help info
 *
 *  format of the .larn.help file
 *
 *  1st character of file:  # of pages of help available (ascii digit)
 *  page (23 lines) for the introductory message (not counted in above)
 *  pages of help text (23 lines per page)
 */
extern char helpfile[];
help()
    {
    register int i,j,maxj;
    char tmbuf[128];    /* intermediate translation buffer when not a VT100 */

    /* open the help file and get # pages 
    */
    if ((j=openhelp()) < 0)  
    return(0);

    /* skip over intro message 
    */
    for (i=0; i<23; i++) 
    lgetl();

    /* if command mode, skip over the second page (prompt mode help)
    */
    if (!prompt_mode)
    {
    for (i=0; i<23; i++)
        lgetl();
    j--;
    }

    for (maxj = j;  j>0; j--)
    {
    clear();
    for (i=0; i<23; i++)
        { 
        tmcapcnv(tmbuf,lgetl());  
        lprcat(tmbuf); 
        } /* intercept \33's */
    if (j>1)
        {
        lprcat("    ---- Press ");  standout("return");
        lprcat(" to exit, ");       standout("space");
        lprcat(" for more help ---- ");
        i=0; while ((i!=' ') && (i!='\n') && (i!='\33')) i=ttgetch();
        if ((i=='\n') || (i=='\33'))
        {
        lrclose();  
        setscroll();  
        drawscreen();  
        return(0);
        }
        }

    /* For prompt mode, skip over the third page (command mode help)
       This could be done more efficiently, but its not worth the trouble.
    */
    if ((prompt_mode) && (j==maxj))
        {
        for (i=0; i<23; i++)
        lgetl();
        j--;
        }

    }
    lrclose();  
    retcont();  
    drawscreen();
    }

/*
 *  function to display the welcome message and background
 */
welcome()
    {
    register int i;
    char tmbuf[128];    /* intermediate translation buffer when not a VT100 */

    if (openhelp() < 0)  return(0);    /* open the help file */
    clear();
    for(i=0; i<23; i++)
            { tmcapcnv(tmbuf,lgetl());  lprcat(tmbuf); } /* intercept \33's */
    lrclose();  retcont();  /* press return to continue */
    }

/*
 *  function to say press return to continue and reset scroll when done
 */
retcont()
    {
    cursor(1,24); lprcat("Press "); standout("return");
    lprcat(" to continue: ");   while (ttgetch() != '\n');
    setscroll();
    }

/*
 *  routine to open the help file and return the first character - '0'
 */
openhelp()
    {
    if (lopen(helpfile)<0)
        {
        lprintf("Can't open help file \"%s\" ",helpfile);
        lflush(); sleep(4); drawscreen();   setscroll(); return(-1);
        }
    resetscroll();  return(lgetc() - '0');
    }
