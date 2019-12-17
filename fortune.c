/* fortune.c */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "header.h"
#include "player.h"
#include "larndefs.h"

extern char fortfile[];

outfortune()
    {
    char *p;

    lprcat("\nThe cookie was delicious.");
    if (c[BLINDCOUNT])
        return(0);
    if (p=fortune(fortfile))
        {
        lprcat("  Inside you find a scrap of paper that says:\n");
        lprcat(p);
        }
    }

/*
 *  function to return a random fortune from the fortune file
 */
static char *base=0;    /* pointer to the fortune text */
static char **flines=0; /* array of pointers to each fortune */
int fortune_file_fd=0;    /* true if we have load the fortune info */
static int nlines=0;    /* # lines in fortune database */

char *fortune(char *file)
{
 char *p;
	int lines,tmp;
	struct stat stat;
	int retval;

	if (fortune_file_fd==0) {
		/* open the file */
		if ((fortune_file_fd=open(file,O_RDONLY)) < 0)	
			return(0); /* can't find file */

		/* find out how big fortune file is and get memory for it */
		retval = fstat(fortune_file_fd,&stat);
		if ((retval < 0) || ((base=malloc((unsigned)(1+stat.st_size))) 
		     == (char *)NULL)) {
			close(fortune_file_fd); 
			fortune_file_fd= -1; 
			if (base) free(base); 
			return(0); 	/* can't stat file */
		}

		/* read in the entire fortune file */
		if ((stat.st_size = read(fortune_file_fd,(char *)base,stat.st_size))
		     == -1) {
			close(fortune_file_fd); 
			fortune_file_fd= -1; 
			if (base) free(base); 
			return(0); 	/* can't read file */
		}
		close(fortune_file_fd);  
		base[stat.st_size] = 0 ; /* final NULL termination */

	/* count up all the lines (and NULL terminate) to know memory needs */
		for (p=base,lines=0; p<base+stat.st_size; p++) /* count lines */
			if (*p == '\n') { 
				*p= 0;
				lines++;
			}
		nlines = lines;

		/* get memory for array of pointers to each fortune */
		if ((flines=(char**)malloc((unsigned)(nlines*sizeof(char*)))) 
		     == (char **)NULL) {
			if (base) free(base); 
			fortune_file_fd= -1; 
			return(0); /* malloc() failure */
		}

		/* now assign each pointer to a line */
		for (p=base,tmp=0; tmp<nlines; tmp++) {
			flines[tmp]=p;  
			while (*p++); /* advance to next line */
		}
	}

	if (fortune_file_fd > 2)	/* if we have a database to look at */
		return(flines[rund((nlines<=0)?1:nlines)]);
	else 
	    return(0);
}
