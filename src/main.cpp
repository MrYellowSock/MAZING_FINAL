//#define DBG
//#define ignore_start
#include <Arduino.h>
#include <maze.h>
#include <controller.h>
Sunsheild ss;
int spd_turn = 120;



void fuck(){
    //put this where problem is
    ss.stop();
    delay(5000);
}
Color readColorsensor(){
    Color c;
    ss.grabColor(&c.r,&c.g,&c.b);
    return c;
}
void printRGB(Color rc){
    Serial.print("RGB:");
    Serial.print(rc.r);
    Serial.print(" ");
    Serial.print(rc.g);
    Serial.print(" ");
    Serial.print(rc.b);
    Serial.print(" ");
    mazeColors mc = rc.getMazeCol();
    switch (mc)
    {
        case mazeColors::red:
            Serial.print("RED");
            break;
        case mazeColors::green:
            Serial.print("GREEN");
            break;
        case mazeColors::white:
            Serial.print("WHITE");
            break;
        case mazeColors::yellow:
            Serial.print("YELLOW");
            break;
        case mazeColors::unknown:
            Serial.print("UNKNOWN");
            break;
    }
    Serial.println();
}
void loop_dbg(){
    //Serial.println(ss.ir_left());
    //Serial.println(ss.ir_right());
    Serial.print("IR:");
    for (int chan=0; chan<8; chan++) {
        Serial.print(ss.getVal(chan)); Serial.print("\t");
    }
    Serial.println();
    Color rc = readColorsensor();
    printRGB(rc);
    delay(1000);
}
void finish();

int spd = 200;
void turnBack();
void straight_regular(){
    ss.ledON(LED2);
    if (ss.isLine(1) || ss.isLine(2))
    {
        ss.right(spd);
        ss.left(spd);
        delay(1);
        ss.right(spd);
        ss.left(0);
        delay(4);
    }
    else if (ss.isLine(5) || ss.isLine(6))
    {
        ss.right(spd);
        ss.left(spd);
        delay(1);
        ss.right(0);
        ss.left(spd);
        delay(4);
    }
    
    ss.right(spd);
    ss.left(spd);
    delay(4);
    ss.stop();
    delay(1);
    ss.ledOFF(LED2);
}
bool straight(){
    ss.ledON(LED2);
    if( ss.isLine(1) && ss.isLine(2) && ss.isLine(5) && ss.isLine(6) ){
        mazeColors here = readColorsensor().getMazeCol();
        if(here == finish_line)
        {
            finish();
            return true;
        }
        else if(here == mazeColors::yellow || here == mazeColors::red){
            ss.right(-spd_turn);
            ss.left(-spd_turn);
            delay(300);
            ss.stop();

            ss.right(spd_turn);
            ss.left(-spd_turn);
            while (ss.isLine_MID50()){ss.rstIfPressed();}
            turnBack();
            return true;
        }
    }
    else if (ss.isLine(1) || ss.isLine(2))
    {
        ss.right(spd);
        ss.left(spd);
        delay(1);
        ss.right(spd);
        ss.left(0);
        delay(5);
    }
    else if (ss.isLine(5) || ss.isLine(6))
    {
        ss.right(spd);
        ss.left(spd);
        delay(1);
        ss.right(0);
        ss.left(spd);
        delay(5);
    }
    
    ss.right(spd);
    ss.left(spd);
    delay(4);
    ss.stop();
    delay(1);
    ss.ledOFF(LED2);
    return false;
}

int k = 35;
void newstraight(){
    float e=0;
    bool all[8] = {0,ss.isLine(1),ss.isLine(2),0,0,ss.isLine(5),ss.isLine(6),ss.isLine(7)};
    if( all[1] && all[3] && all[5] && all[7] ){
        if(readColorsensor().getMazeCol() == finish_line){
            finish();
            return;
        }
    }
    else if(all[1])e=2;
    else if( all[2])e=1;
    else if(all[5])e=-1;
    else if(all[6])e=-2;
    
    e *=k;
    ss.right(spd+e);
    ss.left(spd-e);
}

#define trash_turn 400 //small error turn

void turn(bool LEFT){
    ss.ledON(LED3);
    timerStart();
    if(LEFT){
        ss.left(-spd_turn);
        ss.right(spd_turn);
    }
    else{
        ss.right(-spd_turn);
        ss.left(spd_turn);
    }
    while (ss.isLine_MID50()){ss.rstIfPressed();}
    while (!ss.isLine_MID100()){ss.rstIfPressed();}
    ss.stop();
    if(timerStop() > trash_turn){
        if(LEFT)add('L');
        else add('R');
    }
    ss.ledOFF(LED3);
}

void turnBack(){
    ss.ledON(LED4);
    timerStart();
    ss.right(spd_turn);
    ss.left(-spd_turn);
    while (!ss.isLine_MID50()){ss.rstIfPressed();}
    ss.stop();
    if(timerStop() > trash_turn){
        add('B');
    }
    ss.ledOFF(LED4);
    ss.right(-spd_turn);
    ss.left(spd_turn);
    delay(80);
    ss.stop();
}

void leftHand(bool L,bool R){// keeping one hand in contact with the wall, 
    if(L || R){
        bool w_l,w_r;
        while ( (w_l = ss.isLine(0) || ss.isLine(1) ) || (w_r = ss.isLine(7) || ss.isLine(6)) )
        {
            ss.rstIfPressed();
            if(straight())return;//color is detected within this func
            L = L || w_l;
            R = R || w_r;//collect all information
        }
    }
    if(L){
        turn(true);
        ss.right(-spd_turn);
        ss.left(spd_turn);
        delay(75);
        ss.stop();
    }
    else if(R){
        if(ss.isLine_MID50()){
            
                 add('S');
            straight_regular();
            return;
        }
        else{
            turn(false);
            ss.right(spd_turn);
            ss.left(-spd_turn);
            delay(50);
            ss.stop();
        }
    }
    else if(!ss.isLine(0) && !ss.isLine(1) && !ss.isLine(2) && !ss.isLine(3) && !ss.isLine(4) && !ss.isLine(5) && !ss.isLine(6) && !ss.isLine(7) ){    
        turnBack();
    }
    else{
        straight_regular();
    }
}
void replay(){
    shortPath();

    //get off start:
    while (ss.isLine(0) || ss.isLine(1) || ss.isLine(7) || ss.isLine(6))
    {
        ss.rstIfPressed();
        ss.left(spd);
        ss.right(spd);
    }

    for(int r=0;r<pathLength;r++){
        bool L,R;
        while( !(L = ss.isLine(0) || ss.isLine(1)) && !(R = ss.isLine(6) || ss.isLine(7)) ){
            ss.rstIfPressed();
            straight_regular();
        }
        if(L || R){
            while ( ss.isLine(0) || ss.isLine(1) || ss.isLine(7) || ss.isLine(6) )
            {
                ss.rstIfPressed();
                straight_regular();
            }
            if(path[r] == 'S'){
                straight_regular();
            }
            else if(path[r] == 'L'){
                turn(true);
                ss.right(-spd_turn);
                ss.left(spd_turn);
                delay(75);
                ss.stop();
            }
            else if(path[r] == 'R'){
                turn(false);
                ss.right(spd_turn);
                ss.left(-spd_turn);
                delay(50);
                ss.stop();
            }
            else if(path[r] == 0){
                ss.left(-spd);
                ss.right(-spd);
                delay(300);
                ss.stop();

                if(readColorsensor().getMazeCol() != finish_line){
                    ss.flagUp();
                    resetPath();
                    ss.waitButton();
                    delay(1000);
                    ss.flagDown();
                    return;
                }
                else{
                    ss.flagUp();
                    finish();
                }
            }
        }
        else{
            r -= 1;
        }
    }
    
}
void finish(){
    ss.flagUp();
    ss.stop();
    storePath();
    ss.waitButton();
    ss.flagDown();
    delay(1000);
    replay();
}
void setup() {
    ss.init();
    Serial.begin(9600);
    showPath();
    ss.flagDown();
    ss.waitButton();
    delay(1000);
    #ifndef DBG
    
    #ifndef ignore_start
    mazeColors mc;
    while((mc = readColorsensor().getMazeCol()) == mazeColors::green){
        ss.left(spd);
        ss.right(spd);
    }
    finish_line = mc;
    while (ss.isLine(0) || ss.isLine(1) || ss.isLine(7) || ss.isLine(6))
    {
        ss.left(spd);
        ss.right(spd);
    }
    #endif    

    //start finish color[DONE].
    //not 100% turn right
    //+timer at turning to cut off error turn.
    //maze shortest replay
    //deal w/ loop
    #endif
}


void loop() {    
    #ifdef DBG
    loop_dbg();
    #else
    bool L=ss.isLine(0),R=ss.isLine(7);
    if(!L && !R && ss.isLine_MID50()){
        straight();
    }
    else{
        leftHand(L,R);
    }
    #endif
}