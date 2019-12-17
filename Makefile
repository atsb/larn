# Add -DSIG_RTNS_INT to the CFLAGS line for older Unixes that return
# int ptr rather than void ptr from the call to signal().
#
# For macOS systems you need -DBSD CFLAG.
# For SunOS/Solaris/HP-UX and GNU/Linux systems
# you need -DSYSV CFLAG.
#
# For GNU/Linux and macOS, you need to remove the -DSYSVSIGNALS
# define which will exclude the SIGEMT signal as it is only
# in SYSV UNIX. It is also used to set the sigset() function call
# instead of sigsetup().
#
# If you are using an ancient UNIX system (doubtful) you may need
# to define -DNOVARARGS to the CFLAGS.
#
# If you wish to have the save file in your home directory
# add -DSAVEINHOME to the CFLAGS (default setup).

BIN = larn

OBJ = action.o bill.o config.o create.o data.o diag.o display.o \
	fortune.o global.o help.o io.o iventory.o main.o monster.o \
	moreobj.o movem.o nap.o object.o regen.o savelev.o \
	scores.o signal.o spells.o spheres.o store.o \
	tok.o

CFLAGS = -g -Wall -Wextra -DSYSV -DEXTRA -DSAVEINHOME #-DSYSVSIGNALS

larn: $(OBJ)
	cc -o $(BIN) $(OBJ) -lncurses

clean:
	rm *.o

.c.o: 
	cc $(CFLAGS) -c $*.c 

.c: header.h larndefs.h player.h monsters.h objects.h patchlev.h
