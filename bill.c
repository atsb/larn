/* bill.c */
#include "header.h"
#include "larndefs.h"
# include "player.h"
# define MAILTMP    "/tmp/#"
static int pid;
static char mail600[sizeof(MAILTMP)+sizeof("mail600")+20];
/*
 *  function to create the tax bill for the user
 */
static int letter1(void)
  {

  sprintf(mail600,"%s%dmail600",MAILTMP,pid); /* prepare path */
  if (lcreat(mail600) < 0) { write(1,"can't write 600 letter\n",23); return(0);}
  lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
  lprcat("From: LRS (Larn Revenue Service)\n");
  lprcat("Subject: Undeclared Income\n\n");
  lprcat("\n   We heard you survived the caverns of Larn.  Let me be the");
  lprcat("\nfirst to congratulate you on your success.  It is quite a feat.");
  lprcat("\nIt must also have been very profitable for you.");
  lprcat("\n\n   The Dungeon Master has informed us that you brought");
  lprintf("\n%d gold pieces back with you from your journey.  As the",(long)c[GOLD]);
  lprcat("\ncounty of Larn is in dire need of funds, we have spared no time");
  lprintf("\nin preparing your tax bill.  You owe %d gold pieces as",(long)c[GOLD]*TAXRATE);
  lprcat("\nof this notice, and is due within 5 days.  Failure to pay will");
  lprcat("\nmean penalties.  Once again, congratulations, We look forward");
  lprcat("\nto your future successful expeditions.\n");
  lwclose();
  return(1);
  }

static int letter2(void)
  {

  sprintf(mail600,"%s%dmail600",MAILTMP,pid); /* prepare path */
  if (lcreat(mail600) < 0) { write(1,"can't write 601 letter\n",23); return(0);}
  lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
  lprcat("From: King (His Majesty King Wilfred of Larndom)\n");
  lprcat("Subject: A Noble Deed\n\n");
  lprcat("\n   I have heard of your magnificent feat, and I, King Wilfred,");
  lprcat("\nforthwith declare today to be a national holiday.  Furthermore,");
  lprcat("\nhence three days, Ye be invited to the castle to receive the");
  lprcat("\nhonour of Knight of the realm.  Upon thy name shall it be written. . .");
  lprcat("\nBravery and courage be yours.");
  lprcat("\nMay you live in happiness forevermore . . .\n");
  lwclose();
  return(1);
  }

static int letter3(void)
  {
  sprintf(mail600,"%s%dmail600",MAILTMP,pid); /* prepare path */
  if (lcreat(mail600) < 0) { write(1,"can't write 602 letter\n",23); return(0);}
  lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
  lprcat("From: Endelford (Count Endelford)\n");
  lprcat("Subject: You Bastard!\n\n");
  lprcat("\n   I heard (from sources) of your journey.  Congratulations!");
  lprcat("\nYou Bastard!  With several attempts I have yet to endure the");
  lprcat(" caves,\nand you, a nobody, makes the journey!  From this time");
  lprcat(" onward, bewarned\nupon our meeting you shall pay the price!\n");
  lwclose();
  return(1);
  }

static int letter4(void)
  {
  sprintf(mail600,"%s%dmail600", MAILTMP,pid); /* prepare path */
  if (lcreat(mail600) < 0) { write(1,"can't write 603 letter\n",23); return(0);}
  lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
  lprcat("From: Mainair (Mainair, Duke of Larnty)\n");
  lprcat("Subject: High Praise\n\n");
  lprcat("\n   With a certainty a hero I declare to be amongst us!  A nod of");
  lprcat("\nfavour I send to thee.  Me thinks Count Endelford this day of");
  lprcat("\nright breath'eth fire as of dragon of whom ye are slayer.  I");
  lprcat("\nyearn to behold his anger and jealously.  Should ye choose to");
  lprcat("\nunleash some of thy wealth upon those who be unfortunate, I,");
  lprcat("\nDuke Mainair, Shall equal thy gift also.\n");
  lwclose();
  return(1);
  }

static int letter5(void)
  {
  sprintf(mail600,"%s%dmail600", MAILTMP,pid); /* prepare path */
  if (lcreat(mail600) < 0) { write(1,"can't write 604 letter\n",23); return(0);}
  lprcat("\n\n\n\n\n\n\n\n\n\n\n\n");
  lprcat("From: StMary (St. Mary's Children's Home)\n");
  lprcat("Subject: These Poor Children\n\n");
  lprcat("\n   News of your great conquests has spread to all of Larndom.");
  lprcat("\nMight I have a moment of a great man's time.  We here at St.");
  lprcat("\nMary's Children's Home are very poor, and many children are");
  lprcat("\nstarving.  Disease is widespread and very often fatal without");
  lprcat("\ngood food.  Could you possibly find it in your heart to help us");
  lprcat("\nin our plight?  Whatever you could give will help much.");
  lprcat("\n(your gift is tax deductible)\n");
  lwclose();
  return(1);
  }

static int letter6(void)
  {
  sprintf(mail600, "%s%dmail600", MAILTMP, pid); /* prepare path */
  if (lcreat(mail600) < 0) { write(1,"can't write 605 letter\n",23); return(0);}
  lprcat("From: CancerSociety (The National Cancer Society of Larn)\n");
  lprcat("Subject: Hope\n\n");
  lprcat("\nCongratulations on your successful expedition.  We are sure much");
  lprcat("\ncourage and determination were needed on your quest.  There are");
  lprcat("\nmany though, that could never hope to undertake such a journey");
  lprcat("\ndue to an enfeebling disease -- cancer.  We at the National");
  lprcat("\nCancer Society of Larn wish to appeal to your philanthropy in");
  lprcat("\norder to save many good people -- possibly even yourself a few");
  lprcat("\nyears from now.  Much work needs to be done in researching this");
  lprcat("\ndreaded disease, and you can help today.  Could you please see it");
  lprcat("\nin your heart to give generously?  Your continued good health");
  lprcat("\ncan be your everlasting reward.\n");
  lwclose();
  return(1);
  }

static int (*pfn[])()= { letter1, letter2, letter3, letter4, letter5, letter6 };
/*
 *  function to mail the letters to the player if a winner
 */
int mailbill(int i)
    {
    char buf[128];
    wait(0);  pid=getpid();
    if (fork() == 0)
        {
        resetscroll();
        for (i=0; i<sizeof(pfn)/sizeof(int (*)()); i++)
            if ((*pfn[i])())
                {
                sleep(20);
                sprintf(buf,"mail %s < %s",loginname,mail600);
                system(buf);  unlink(mail600);
                }
        exit(0);
        }
    }

