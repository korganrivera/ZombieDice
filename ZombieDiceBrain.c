/*
2015.6.2.13:20      Just here to clean up before uploading to Git. The comments below are working notes, not important
                    unless you're into that kind of thing. This code will play Zombie Dice with the user on the command
                    line. The computer has been trained with a simple neural network to be as good as possible against
                    other AIs. I ran a competition, and stuck with the current algorithm when I got bored. It is hard
                    for me to beat the computer player now. This code could easily be adapted to Linux. I had to write
                    this on a windows machine because my linux machine died.

ZOMBIEDICE4
                    Cleaning up the last file, and adding a bunch of new code because I fucked up the previous one.

                    Been doing this shit for about 12 hours now.  doesn't work. the sort routines are fucked. do later. -_-
                    3 days later. 12:13am on 26th may 2015.  728 lines later.  PLEEZ WURK.

                    nope: shotgun received a 4 is my error.  somehow my tables aren't being managed correctly.
                    also, i threw in a arrayindex=0 somewhere which I need to double check.

2015.3.26.14:36:    It finally works! I had some problems, but fixed them. I wasn't clearing the array[][] before it
                    was reused, and so that's fine now. This took 3 days and about 36 hours of work, 744 lines of code
                    right now. I can probably trim code here and there and make it more efficient. I thought of an
                    improvement though. The decision right now is entirely based on the risk of death. Perhaps it should
                    take into account the number of brains on the table being risked. If there is a 49% of death, but 10
                    brains on the table, then it might be better not to roll. But if there is a 80% chance of death, and
                    no brains on the table, then you're not risking anything by playing really. So I need to come up
                    with some risk calculation to balance the two. Probably need some kind of learning mechanism. But it
                    works for now! :D

                    In another program, I had this algorithm play against a version of the computer
                    that just made random moves. So, it turns out the computer performed best when it had a
                    riskthreshhold of 0.21. I don't know why. Something about taking risks with a certain number of
                    brains on the table I'm sure.But I've changed this code to have 0.21 and maybe that will be
                    awesome.

                    I've changed it to 0.23 because that beats a player who always rolls again with less than 2
                    shotguns on the table.The only way to beat this I think is to have a neural network take other
                    inputs into account.Also, this has only been simulated to see which technique will gather more
                    points, not win more games. I assumed the problems are equivalent, butmaybe they're not.

                    Okay. I did another experiment. Not only does the program take into account the risk of death,
                    it also considers the brains on the table.If risk is too high and brains on table<=5, then it will roll again.

2015.4.14.00:05     Okay!  SO I just finished a couple of runs with my perceptron, and so I have a new function for the computer player.
                    The new function is uses the e version of the sigmoid function, and it's sige(x) where x = 7.4(brain()) -1.9(brains on table) +1.9.
                    If this function >= 0.5, then roll again, otherwise don't.

2015.4.14.02:32    I changed the brain() function to instead return the raw risk value, instead of the 0 or 1 thing.  Then I recalculated using the
                    perceptron thing, and got some new numbers.  The computer is a pretty good player now, and it's harder to beat.
                    But i still want to understand the back propagation algorithm so I can calculate better values faster, instead of waiting
                    an hour and a half.

2015.4.14.23:35     Okay, in this version I'm going to try to use a learning algorithm instead of just iterating for good values. ALso, I'm taking out the
                    sigmoid function which seems pointless since a perceptron can work without it.
                    So I'll use the thing to make a prediction.  I'll roll the dice.  If I die...I don't know if this will work.
                    I need to know if given weights are more likely to help you win games.  not win rolls.

                    I think what I'll do is scrap the sigmoid function, use a threshhold of zero.  If w1*risk+w2*brains>=0 then 1, else 0.
                    Iterate as usual through the loops, but this way will calculate faster since I don't need all those function calls.
                    Inline Siga() is fastest. if I do use sigmoid.
*/

//  compile in windows.  uses system().
//  @author: Korgan Rivera (korganrivera@gmail.com)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <math.h>

#define RED 0
#define GREEN 1
#define YELLOW 2
#define BRAIN 0
#define RUNNER 1
#define SHOTGUN 2
#define RISKTHRESHHOLD 0.23             //  if computer calculates risk to be greater than this, it won't roll again. make this dynamic later.
#define SKIP 4
#define BRAINLIMIT 13                   //  Number of brains you need to get to win.


typedef struct {
    unsigned brains[15];
    unsigned runners[3];
    unsigned shotgun[5];
    unsigned bi,ri,si;                  //  indices for the arrays above.
    unsigned score;                     //  this holds points from brain dice I had to roll.
}_table;

_table table;

//format is 1stdicecolor 2nddicecolor 3rddicecolor number-of-duplicate-combos
unsigned array[286][4];
unsigned arrayindex=0;
unsigned player[2];                     //  player scores.
unsigned cup[13]={2,2,2,1,1,1,1,1,1,2,0,0,0};
unsigned cupindex=0;

unsigned roll(unsigned dcolour);            //  rolls a dice of a given colour. returns code for a dice face, given the colour.
void resettable(void);                      //  take dice off table, put them back in the cup[].
void displaytable(void);                    //  show the table.
void turnannounce(unsigned n);              //  announce whose turn it is.
float brain(void);                          //  returns 1 if brain thinks "yes roll again" or 0 if "nope".
float shotgunchance(unsigned dcolour);      //  returns probability of rolling a shotgun with the given dice. just a lookup table.
void vswap(unsigned row1, unsigned row2);   //  swaps two given rows vertically.
float cupchance(unsigned a, unsigned b);    //  calculates chance of getting b shotguns when taking 'a' dice from the cup.
void combinationUtil(unsigned arr[], unsigned n, unsigned r, unsigned index, unsigned data[], unsigned i);
void storeCombination(unsigned arr[], unsigned n, unsigned r);
void display(unsigned n);                   //  displays n rows of the array.
unsigned rowmatch(unsigned a, unsigned b);  //  tells me if two unsorted row entries match,
void dupesort(unsigned n);                  //  sort the array by number of duplicates, from greatest to least.
void shuffle(void);
long unsigned factorial(long unsigned n);
unsigned compright=0, comptotal=0;          //  Keep track of how often the computer's decision worked out well.
unsigned userright=0, usertotal=0;          //  Keep track of how often the user's decision worked out well.
double Sige(double f);                      //  sigmoid functino for perceptron.
//  use these last two later, I can't concentrate right now.

int main(void){
    unsigned rolls;                         //  number of rolls taken in current turn.
    unsigned pflag=0;                       //  indicates which player is currently active. 0=user, 1=computer.
    unsigned runnercount=0;                 //  number of runners on table that are open to reroll.
    unsigned r;                             //  tells me what face showed up on a rolled dice.
    char in;                                //  input from user for 'roll again?'
    unsigned i,j,k;                         //  slutty loop variable.

    srand(time(NULL));
    shuffle();

    system("cls");
    puts("\nZOMBIE DICE!   F(x_x)F \n");

    //initialise array[].
    for(i=0; i<286; i++) array[i][0]=array[i][1]=array[i][2]=array[i][3]=0;

    //begin the infinite zombie onslaught!
    for(;;){
            turnannounce(pflag);
            rolls=0;
            while(rolls<3){
                if(runnercount){                            //  if there are runners on the table, roll them.
                    printf("\nchasing a runner! ");
                    table.ri--;                             //  remove runner dice from table.
                    r=roll(table.runners[table.ri]);        //  reroll that dice.
                    runnercount--;
                }
                else if(cupindex<13){                       //  else, if there is a dice in the cup, roll it.
                    printf("\nrolling from cup. ");
                    r=roll(cup[cupindex]);
                    cupindex++;
                }
                else{                                       //  otherwise, roll a brain on the table.
                    if(table.bi){
                        printf("\ncup empty. rolling brains on table. ");
                        table.score++;                      //  log point from brain dice I'm about to reuse.
                        table.bi--;                         //  remove brain from table.
                        r=roll(table.brains[table.bi]);     //  reroll that brain.
                    }
                    else{                                   //  if there are no brains either, then the game is fucked. prob won't happen.
                        puts("\nno dice to roll.  game has problem. exiting.");
                        exit(1);
                    }
                }
                if(r==RUNNER)  printf("You rolled a runner.");
                else if(r==BRAIN) printf("You ate a brain!");
                else printf("You got blasted!");
                rolls++;
            }

            displaytable();

            if(table.si>=3){                                            //  if 3+ shotgun blasts, end of turn.
                if(!pflag) puts("\nBoom! You died.");
                else puts("\nComputer has died.");
                resettable();
                pflag ^=1;                                              //  switch players: toggle pflag with XOR.
                printf("Press a key to continue."); getch(); system("cls"); //bleugh!
            }

            else if(player[pflag]+table.bi+table.score>=BRAINLIMIT){    //  if current player has 13+ brains, he wins. end game.
                if(!pflag) printf("\nYou win with %u brains!\n", player[pflag]+table.bi+table.score);
                else printf("\nComputer wins with %u brains!\n", player[pflag]+table.bi+table.score);
                exit(0);
            }

            else{
                if(!pflag){ //if player is user, get decision.
                    printf("\nRoll again? (y/n): ");
                    fflush(stdin);
                    in = getc(stdin);
                }

                else{  //   computer decides to roll again or not.
            //  if(brain() && table.bi<=5) { in='y'; puts("\nComputer decides to roll again."); }
            //  else{ in='n'; puts("\nComputer decides to not roll."); }

                    //  new much better perceptron-determined version!
                    if(Sige(-5.0 * brain() - 2.4 * (float)table.bi + 8)>=0.5) { in='y'; puts("\nComputer decides to roll again."); }
                    else{ in='n'; puts("\nComputer decides to not roll."); }
                }

                if(in=='Y' || in=='y'){
                        runnercount=table.ri;       //  this makes runners available to reroll on the next turn.
                        if(pflag){ printf("Press a key to continue."); getch(); puts("");}              //  bleugh!
                        system("cls");
                        continue;
                }

                else{
                        player[pflag]+=table.bi;
                        resettable();
                        pflag^=1;
                        printf("Press a key to continue."); getch(); system("cls"); //  vomit.
                    }

            }
     }//    end of infinite game loop.
}


unsigned roll(unsigned dcolour){
    unsigned r=rand()%6;

    if(r<2) { table.runners[table.ri++]=dcolour; return RUNNER; }
    if(dcolour==RED){
            if(r<5){ table.shotgun[table.si++]=RED; return SHOTGUN; }
            else{ table.brains[table.bi++]=RED; return BRAIN; }
    }
    else if(dcolour==GREEN){
            if(r<3){ table.shotgun[table.si++]=GREEN; return SHOTGUN; }
            else{ table.brains[table.bi++]=GREEN; return BRAIN; }
    }
    else if(dcolour==YELLOW){
        if(r<4){ table.shotgun[table.si++]=YELLOW; return SHOTGUN; }
        else{ table.brains[table.bi++]=YELLOW; return BRAIN; }
    }
    else {
        puts("\nroll() got sent a value it can't handle!");     //  probably unnecessary.
        exit(1);
    }
}


void shuffle(void){
    unsigned i, j, temp;

    //shuffle the dice, Fisher-Yates style.
    puts("\nshuffling dice in cup.");
    for(i=12; i; i--){
        j = rand()%(i+1);
        temp = cup[j];
        cup[j]=cup[i];
        cup[i]=temp;
    }
}


void resettable(void){
    printf("\nresetting the table.");
    while(table.bi){    //  move brains from table to cup.
        table.bi--;
        cupindex--;
        cup[cupindex]=table.brains[table.bi];
    }
    while(table.ri){    //  move runners from table to cup.
        table.ri--;
        cupindex--;
        cup[cupindex]=table.runners[table.ri];
    }
    while(table.si){    //  move shotguns from table to cup.
        table.si--;
        cupindex--;
        cup[cupindex]=table.shotgun[table.si];
    }
    shuffle();
    cupindex=0;
    table.score=0;
}


void displaytable(void){
    unsigned i;

    //display cup content for debug.
    //puts("\ncup contents: ");
    //for(i=cupindex; i<13; i++){
    //        if(cup[i]==RED) printf("R ");
    //        else if(cup[i]==GREEN) printf("G ");
    //        else printf("Y ");
    //}

    puts("\n__________________________________");
    printf("SHOTGUN:");
    for(i=0; i<table.si; i++){
        if(table.shotgun[i]==RED) printf(" R");
        else if(table.shotgun[i]==GREEN) printf(" G");
        else printf(" Y");
    }
    puts("\n__________________________________");
    printf("RUNNERS:");
    for(i=0; i<table.ri; i++){
        if(table.runners[i]==RED) printf(" R");
        else if(table.runners[i]==GREEN) printf(" G");
        else printf(" Y");
    }
    puts("\n__________________________________");
    printf("BRAINS:\t");
    for(i=0; i<table.bi; i++){
        if(table.brains[i]==RED) printf(" R");
        else if(table.brains[i]==GREEN) printf(" G");
        else printf(" Y");
    }
    for(i=0; i<table.score; i++) printf(" x");   // this displays an 'x' to represent a point from a brain that had to be rerolled.

puts("\n__________________________________");
    printf("\nYOU: %u\tCOMP: %u\n", player[0], player[1]);
puts("__________________________________");

}


void turnannounce(unsigned n){
    if(n) puts("\nCOMPUTER'S TURN~~~~~~~~~~~~~~~~~~~");
    else  puts("\nUSER'S TURN~~~~~~~~~~~~~~~~~~~~~~~");
}


//  returns computer's yes or no decision. 1=yes, 0 = no.
float brain(void){
	float deathchance=0.0;

    //printf("\n----------------------Entering the brain");
    //printf("\nrunners=%u and shotguns=%u", table.ri, table.si);

    //deal with the situation when you need more dice from the cup than exists.
    //for example, 12 brains, no runners, 1 dice in cup. Or, 2 shotguns, 2 runners, 9 brains, 0 in cup.
    //NOTE: have to roll brains at end of the brain array, which is a hassle.

    //  if not enough dice in the cup, do this.
    if((table.ri<3) && (3-table.ri > (13-cupindex))){
        if(!table.ri){
            if(!(13-cupindex)){
                if(!table.si){  //  no runners, no cup, no shotguns.
                    //  calculate for 3 brain roll.
                    deathchance = shotgunchance(table.brains[table.bi-1])*shotgunchance(table.brains[table.bi-2])*shotgunchance(table.brains[table.bi-3]);
                }
                else if(table.si==1){   //  no runners, no cup, 1 shotgun.
                   //   calculate chance of at least 2 shotguns with 3 brains.
                    float A,B,C,D,E,F,G;
                    A = shotgunchance(table.brains[table.bi-1]);
                    B = shotgunchance(table.brains[table.bi-2]);
                    C = shotgunchance(table.brains[table.bi-3]);

                    D = A*(1-B)*C;
                    E = (1-A)*B*C;
                    F = A*B*(1-C);
                    G = A*B*C;

                    deathchance = D+E+F+G;
                }
                else if(table.si==2){   //  no runners, no cup, 2 shotguns.
                    //  calculate chance of at least 1 shotgun with 3 brain rolls.
                    deathchance = 1-((1-shotgunchance(table.brains[table.bi-1]))*(1-shotgunchance(table.brains[table.bi-2]))*(1-shotgunchance(table.brains[table.bi-3])));
                }
            }
            else if((13-cupindex)==1){  //  1 dice in cup, no runners.
                if(!table.si){  //  no runners, 1 in cup, no shotguns.
                    //  3 shotguns from 1 in cup, 2 brains.
                    deathchance = shotgunchance(table.brains[table.bi-1])*shotgunchance(table.brains[table.bi-2])*cupchance(1,1);
                }
                else if(table.si==1){   //  no runners, 1 in cup, 1 shotgun.
                    //  at least 2 shotguns from 1 cup, 2 brains.
                    float A,B,C,D,E,F,G;

                    A = shotgunchance(table.brains[table.bi-1]);
                    B = shotgunchance(table.brains[table.bi-2]);
                    C = cupchance(1,1);

                    D = A*(1-B)*C;
                    E = (1-A)*B*C;
                    F = A*B*(1-C);
                    G = A*B*C;

                    deathchance = D+E+F+G;
                }
                else if(table.si==2){   //  no runners, 1 in cup, 2 shotguns.
                    //  at least 1 shotgun from 1 cup, 2 brains.
                    deathchance = 1-((1-shotgunchance(table.brains[table.bi-1]))*(1-shotgunchance(table.brains[table.bi-2]))*(1-cupchance(1,1)));
                }
            }
            else if((13-cupindex)==2){  //  2 dice in cup.
                if(!table.si){  //  no runners, 2 in cup, no shotguns.
                    //  3 shotguns from 2 in cup, 1 brains.
                    deathchance = shotgunchance(table.brains[table.bi-1])*cupchance(2,2);
                }
                else if(table.si==1){   //  no runners, 2 in cup, 1 shotgun.
                    //  at least 2 shotguns from 2 cup, 1 brains.
                    float A,B,C,D,E,F;

                    A = shotgunchance(table.brains[table.bi-1]);
                    B = cupchance(2,1);
                    C = cupchance(2,2);

                    D = A*B;
                    E = A*C;
                    F = (1-A)*C;

                    deathchance = D+E+F;
                }
                else if(table.si==2){   //no runners, 2 in cup, 2 shotguns.
                    //at least 1 shotgun from 2 cup, 1 brains.
                    deathchance = 1-((1-shotgunchance(table.brains[table.bi-1]))*(1-cupchance(2,1)-cupchance(2,2)));
                }

            }
        }

        else if(table.ri==1){
            if(!(13-cupindex)){
                if(!table.si){  //1 runners, no cup, no shotguns.
                    //3 shotguns from 1 runner, 2 brain roll.
                    deathchance = shotgunchance(table.runners[0])*shotgunchance(table.brains[table.bi-1])*shotgunchance(table.brains[table.bi-2]);
                }
                else if(table.si==1){   //1 runners, no cup, 1 shotgun.
                   //calculate chance of at least 2 shotguns with 1 runner, 2 brains.
                }
                else if(table.si==2){   //1 runners, no cup, 2 shotguns.
                    //calculate chance of at least 1 shotgun with 1 runner, 2 brains.
                    deathchance = 1-((1-shotgunchance(table.runners[0]))*(1-shotgunchance(table.brains[table.bi-1]))*(1-shotgunchance(table.brains[table.bi-2])));
                }
            }
            else if((13-cupindex)==1){  //1 dice in cup, 1 runners.
                if(!table.si){  //1 runners, 1 in cup, no shotguns.
                    //calculate chance of 3 shotgun with 1 runner, 1 cup, 1 brains.
                    deathchance = shotgunchance(table.runners[0])*shotgunchance(table.brains[table.bi-1])*cupchance(1,1);
                }
                else if(table.si==1){   //1 runners, 1 in cup, 1 shotgun.
                    //at least 2 shotguns from 1 runner, 1 cup, 1 brains.
                }
                else if(table.si==2){   //1 runners, 1 in cup, 2 shotguns.
                    //calculate chance of at least 1 shotgun with 1 runner, 1 cup, 1 brains.
                    deathchance = 1-((1-shotgunchance(table.runners[0]))*(1-shotgunchance(table.brains[table.bi-1]))*(1-cupchance(1,1)));
                }
            }
        }

        else if(table.ri==2){
            if(!(13-cupindex)){
                if(!table.si){  //2 runners, no cup, no shotguns.
                    //calculate for 2 runner, 1 brain roll.
                    deathchance = shotgunchance(table.runners[0])*shotgunchance(table.runners[1])*shotgunchance(table.brains[table.bi-1]);
                }
                else if(table.si==1){   //2 runners, no cup, 1 shotgun.
                   //calculate chance of at least 2 shotguns with 2 runner, 1 brains.
                }
                else if(table.si==2){   //2 runners, no cup, 2 shotguns.
                    //calculate chance of at least 1 shotgun with 2 runner, 1 brains.
                    deathchance = 1-((1-shotgunchance(table.runners[0]))*(1-shotgunchance(table.runners[1]))*(1-shotgunchance(table.brains[table.bi-1])));
                }
            }
        }
    }//end of lacking cup situation.

    //otherwise, if there are plenty of dice in the cup, do this stuff instead.
    else{
        if(table.ri==3){
            if(table.si==0){
                deathchance = shotgunchance(table.runners[0]) * shotgunchance(table.runners[1]) * shotgunchance(table.runners[2]);
            }

            else if(table.si==1){
                float A,B,C,D,E,F,G;
                A = shotgunchance(table.runners[0]);
                B = shotgunchance(table.runners[1]);
                C = shotgunchance(table.runners[2]);

                D = A*(1-B)*C;
                E = (1-A)*B*C;
                F = A*B*(1-C);
                G = A*B*C;

                deathchance = D+E+F+G;
            }
            else if(table.si==2){
                deathchance = 1-((1-shotgunchance(table.runners[0]))*(1-shotgunchance(table.runners[1]))*(1-shotgunchance(table.runners[2])));
            }
        }
        else if(table.ri==2){
            if(table.si==0){
                deathchance = shotgunchance(table.runners[0]) * shotgunchance(table.runners[1]) * cupchance(1,1);
            }
            else if(table.si==1){
                float A,B,C,D,E,F,G;

                A = shotgunchance(table.runners[0]);
                B = shotgunchance(table.runners[1]);
                C = cupchance(1,1);

                D = A*(1-B)*C;
                E = (1-A)*B*C;
                F = A*B*(1-C);
                G = A*B*C;

                deathchance = D+E+F+G;
            }
            else if(table.si==2){
                float A,B,C,D,E,F,G;

                A = shotgunchance(table.runners[0]);
                B = shotgunchance(table.runners[1]);
                C = cupchance(1,1);

                deathchance = 1 - ((1-A)*(1-B)*(1-C));
            }
        }
        else if(table.ri==1){
            if(table.si==0){
                deathchance = shotgunchance(table.runners[0]) * cupchance(2,2);
      //          printf("\nrunnershotgunchance=%f, cupchance(2,2)=%f", shotgunchance(table.runners[0]), cupchance(2,2));
            }
            else if(table.si==1){
                float A,B,C,D,E,F;

                A = shotgunchance(table.runners[0]);
                B = cupchance(2,1);
                C = cupchance(2,2);

                D = A*B;
                E = A*C;
                F = (1-A)*C;

                deathchance = D+E+F;
            }
            else if(table.si==2){
                float A,B,C,D,E,F,G,H;

                A = shotgunchance(table.runners[0]);
                B = cupchance(2,1);
                C = cupchance(2,2);

                D = (1-A)*B;
                E = A*B;
                F = (1-A)*C;
                G = A*C;
                H = A*(1-B-C);

                deathchance = D+E+F+G+H;
            }
        }
        else if(table.ri==0){
            if(table.si==0){
                deathchance = cupchance(3,3);
            }
            else if(table.si==1){
                deathchance = cupchance(3,2) + cupchance(3,3);
            }
            else if(table.si==2){
                deathchance = cupchance(3,1) + cupchance(3,2) + cupchance(3,3);
            }
        }
    }
    printf("\ndeathchance = %.3f%%", deathchance*100.0);

    return deathchance;
    //if(deathchance>RISKTHRESHHOLD) return 0;
    //if(deathchance<RISKTHRESHHOLD) return 1;

    //if deathchance is exactly 50%, return a random decision.
    //maybe chance this decision process later, so that it takes into account the number of points it's risking.
    //so if tons of brains on table, and 50% risk of dying, don't reroll. but if 50% chance of dying and only 1 brain, maybe reroll.
    //that might require a tiny neural net-style calculation.
    return rand()%2;
}


float shotgunchance(unsigned dcolour){
    if(dcolour==RED) return 0.5;
    if(dcolour==GREEN) return (1.0/6.0);
    if(dcolour==YELLOW) return (1.0/3.0);
    printf("\nshotgun chance received '%u'", dcolour);
    //puts("shotgunchance() got something weird!!!");
}


float cupchance(unsigned a, unsigned b){
    unsigned cupsize;
    unsigned i,j;
    unsigned combinations;
    unsigned uniquecombinations;
    float sum;
    float *array1, *array2;

    //how many dice are in the cup?
    cupsize = 13-cupindex;

    //make sure there are enough dice in the cup.
    if(a>cupsize){ printf("\ncupchance(): can't take %u dice from %u-dice cup. brain-rolling situation. exit(1)", a, cupsize); exit(1); }
    if(!cupsize) { printf("\ncupchance(): no dice in cup. brain-rolling situation. exit(1)", a, cupsize); exit(1); }

    if(!a){ printf("\ncupchance(): I was asked to take zero dice from cup. exiting(1)"); exit(1); }
    if(a>3) { printf("\ncupchance(): I was asked to take more than 4 dice from cup. exiting(1)"); exit(1); }

    if(!b){ printf("\ncupchance(): I was asked for chance of zero shotguns. exiting(1)"); exit(1); }
    if(b>3){ printf("\ncupchance(): I was asked for chance of more than 3 shotguns. exiting(1)"); exit(1); }

    if(b>a)  { printf("\ncupchance(): I can't get %u shotguns if I only take %u dice, duh.", b,a); return 0; }

	//If I take 'a' dice from the cup, how many combinations are there?
    //This block is here to avoid calculating 13!, which is a 33-bit calculations. :/
	if(cupsize==13){
        if(a==3) combinations = 286;
        else if(a==2) combinations = 78;
        else if(a==1) combinations = 13;
    }

    else combinations = factorial(cupsize) / (factorial(cupsize-a) * factorial(a));

    //printf("\ncupsize = %u. Combinations of length %u = %u\n", cupsize, a, combinations);

    //reset arrayindex. yeah?
    arrayindex=0;

    //put all those combinations into array[][].
	storeCombination(&cup[cupindex], cupsize, a);

    //clear old duplicate counts first, so I don't accidentally use old counts for skipped entries.
    for(i=0; i<combinations; i++) array[i][3] = 0;

    //count duplicates.
    for(i=0; i<combinations; i++){
        if(array[i][0]==SKIP) continue;
        array[i][3] = 1;    //Makes sure the combination counts itself.
        for(j=i+1; j<combinations; j++){
            if(array[j][0]==SKIP) continue;
            if(rowmatch(i,j)){
                array[i][3]++;
                array[j][0]=SKIP;
            }
        }
    }

    //sort the rows of the list by duplicate number, from highest to lowest.
    dupesort(combinations);
    //display(combinations);

    //count the number of unique combinations.
    uniquecombinations=i=0;
    while(array[i][0]!=SKIP && i<combinations){ uniquecombinations++; i++; }

    //printf("\nunique combinations: %u", uniquecombinations);

    //I'll malloc() space here for array1 and array2, so they're of length uniquecombinations.
    if((array1=malloc(uniquecombinations*sizeof(float))) == NULL){
        printf("Error allocating memory to array 1!\n");
        exit(1);
    }

    if((array2=malloc(uniquecombinations*sizeof(float))) == NULL){
        printf("Error allocating memory to array 2!\n");
        exit(1);
    }

    //initialise arrays,
    for(i=0; i<uniquecombinations; i++) array1[i] = array2[i] = 0.0;

    //for each combo, calculate it's chance of being taken from the bag. Put result in array1.
    for(i=0; i<uniquecombinations; i++) array1[i] = (float)array[i][3] / (float)combinations;

    //calculate probability of getting 'b' shotguns with that combination.
    for(i=0; i<uniquecombinations; i++){
		array2[i] = shotgunchance(array[i][0]);
		if(b>1) array2[i] *= shotgunchance(array[i][1]);
		if(b>2) array2[i] *= shotgunchance(array[i][2]);
	}

    //display array1 and array2 for debug.
    //puts("\n\ncombo\tshotgun");
    //for(i=0; i<uniquecombinations; i++){
    //    printf("\n%u) %.5f\t%.5f", i, array1[i], array2[i]);
    //}

    //multiply array1[] and array2[] together and sum all the products.
    sum = 0.0;
    for(i=0; i<uniquecombinations; i++) sum += array1[i]*array2[i];
    //printf("\n\nsum = %f", sum);
    free(array1);
    free(array2);

    //printf("\nsummary: if I take %u dice from this cup of %u dice, there is a %.2f%% chance of getting %u shotguns.", a, cupsize, sum*100.0, b);

    return sum;
}


void dupesort(unsigned n){
        unsigned i,j, c;

        //Selection sort.
        for(i=0; i<n; i++){
            c = i;
            for(j=i+1; j<n; j++) if(array[c][3] < array[j][3]) c = j;
            if(i!=c) vswap(i,c);
        }
}


void vswap(unsigned row1, unsigned row2){
    unsigned i, temp;

    for(i=0; i<4; i++){
        temp = array[row1][i];
        array[row1][i] = array[row2][i];
        array[row2][i] = temp;
    }
}


long unsigned factorial(long unsigned n){
  if (!n) return 1;
  return(n * factorial(n-1));
}


void storeCombination(unsigned arr[], unsigned n, unsigned r){
    // A temporary array to store all combination one by one
    unsigned data[r];

    // print all combination using temporary array 'data[]'
    combinationUtil(arr, n, r, 0, data, 0);
}


/* arr[]  ---> Input Array
   n      ---> Size of input array
   r      ---> Size of a combination to be printed
   index  ---> Current index in data[]
   data[] ---> Temporary array to store current combination
   i      ---> index of current element in arr[]     */
void combinationUtil(unsigned arr[], unsigned n, unsigned r, unsigned index, unsigned data[], unsigned i){

    // Current combination is ready, store it
    if (index == r){
        for(long unsigned j=0; j<r; j++) array[arrayindex][j]=data[j];
        arrayindex++;
        return;
    }

    // When no more elements are there to put in data[]
    if (i >= n) return;

    // current is included, put next at next location
    data[index] = arr[i];
    combinationUtil(arr, n, r, index+1, data, i+1);

    // current is excluded, replace it with next (Note that
    // i+1 is passed, but index is not changed)
    combinationUtil(arr, n, r, index, data, i+1);
}


void display(unsigned n){
    unsigned i,j;

    printf("\t  0 | 1 | 2 | 3\n_______________________");

    for(i=0; i<n; i++){
        printf("\nrow %u|\t", i);
        for(j=0; j<4; j++) { if(array[i][j]>=10) printf(" %u ", array[i][j]); else printf("  %u ", array[i][j]);}
    }
}


unsigned rowmatch(unsigned a, unsigned b){
    unsigned c,d,e,f,g,h,i,j,k;

    //I know this looks weird, but it works.
    //It compares 3 elements on row a, with 3 elements on row b to see if they contain the same elements,
    //despite not being in the same order.
    //Doing it this way saves me from sorting the array first.

    c = (array[a][0] == array[b][0]);
    d = (array[a][1] == array[b][1]);
    e = (array[a][2] == array[b][2]);
    f = (array[a][1] == array[b][2]);
    g = (array[a][2] == array[b][1]);
    h = (array[a][0] == array[b][1]);
    i = (array[a][2] == array[b][0]);
    j = (array[a][0] == array[b][2]);
    k = (array[a][1] == array[b][0]);

    if((c && d && e) || (c && f && g) || (h && f && i) || (j && d && i) || (h && k && e) || (j && k && g)) {
//        printf("\n%u%u%u and %u%u%u match.\n", array[a][0], array[a][1], array[a][2], array[b][0], array[b][1], array[b][2]);
        return 1;
    }
    return 0;
}


double Sige(double f){
    return 1.0/(1.0 + exp(-f));
}
