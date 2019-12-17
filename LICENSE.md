# License

The below is the original posting pertaining to the future of Larn, by Noah Morgan,
original author and copyright holder:

Relay-Version: version B 2.10 5/3/83; site utzoo.UUCP
Path: utzoo!watmath!clyde!caip!topaz!husc6!harvard!panda!condor!noah
From: noah@condor.UUCP (Noah Morgan)
Newsgroups: net.sources.games,net.games
Subject: larn and coming attractions
Message-ID: <1288@condor.UUCP>
Date: Thu, 10-Jul-86 10:56:04 EDT
Article-I.D.: condor.1288
Posted: Thu Jul 10 10:56:04 1986
Date-Received: Sat, 12-Jul-86 03:09:14 EDT
Distribution: net
Organization: GenRad, Inc., Bolton, Mass.
Lines: 43
Xref: watmath net.sources.games:685 net.games:2998

-----------[ Eat this, you Demon lord! ]-------------------------------------

OK folks.  Its time to clarify the future of larn.  There have been sufficient
comments and inquiries about it that I feel I should say whats in the works.
(It might also avoid some people trying to do what I am doing, ie duplication
of effort).  Version 12.0 of larn will be released in about 2 weeks.  It will
use termcap for those of you that don't have any VT100 type terminals (#1
complaint).  Thanks here goes to Michiel Huisjes for a clean termcap patch.
Larn 12.0 will be able to use playerid's instead of userid's for scoreboard
management.  This allows those sites with several people playing from one uid to
have individual scoreboard entries based on their unique playerid #.  Much of
the code has been rewritten to 1. make it faster and more bullet proof, 2. make
it more portable.  An attempt was made to fix the problems with unsigned chars
only on machines, as well as those 16 bit ints.  I now use shorts and longs,
respectively.  Intelligent monster movement has been added for those monsters
smart enough to use it. (This changes the nature of the game a little). 
A .holiday file has been added.  Many bugs have been fixed, and several new
items have been added to the game, and overall, a general rewrite.
Larn has been run through lint, and the real problems it reports are being
tended to.  If you have any suggestions for improvement of larn, feel free
to email me your suggestions.  I will consider them.  Please leave your
rudeness and name calling in YOUR mbox, not mine.

I'd also like to clarify my position on the source code to larn:

Larn was distributed to the net for the enjoyment of all.  One of my goals
is to have larn available to whom ever wants to play it.  I therefore give
permission to use the sources, to modify the sources, or to port the sources to
another machine, provided that a profit is not made from larn or its sources,
or the aforementioned activities.  Should a profit be made without permissions,
I will exercise my copyright.  Other than this case, enjoy it!

One more thing:  I will not be porting larn to PC's or Amiga's.  Other 
poeple on the net have expressed an intention to do this, and I will leave it
in their hands.  I also leave it to those poeple to notify the net if they
develop such a version.


			  ___	Prince of Gems (alias Noah Morgan)
			 /.  \	panda!condor!noah
			 \   /	at GenRad Inc.  Bolton MA
			  \ /
			   v
