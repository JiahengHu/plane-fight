/* Art of Engineering, Columbia Project
 * arthur: Jiaheng Hu
 * 
 * This program is a plane fighting game, presented as the common project for ZZXFD team.
 * 
 * Thanks to Zichuan Wang and the ST7565 Library
 * 
 * 
 * Special features：Picture of Plane and picture of missile
 *                   scale is easy to modify
 *                   seed with the current time so that random is not fake anymore
 *                   speed of missile is proportional to the score, but within a reasonable range
 *                   show instant score as well as the highest score
 */

#include "ST7565.h"


/*************************************** Setting up **************************************************************/
//setting up the button
const int BL = 3;     // the number of the left pushbutton pin
const int BR = 4;     // the number of the right pushbutton pin

//button number recorder
int state1,state2;


// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
ST7565 glcd(9, 8, 7, 6, 5);


int high=0;//highest score
int loopCount=0;

/*************************************** Set up function **************************************************************/
// The setup() method runs once, when the sketch starts
void setup()   {                
   Serial.begin(9600);

#ifdef __AVR__
  Serial.print(freeRam());
#endif
  
  // initialize and set the contrast to 0x18
  glcd.begin(0x18);
  glcd.clear();
  /*
  // draw a string at location (0,0)
  glcd.drawstring(0, 0, "Plane Fight V3.5                          Presented by ZZXFD");
  glcd.display();
  delay(3000);
  glcd.clear();

  glcd.drawstring(0, 0, "You are the pilot of a B-2 bomber and your mission is to avoid the enemy missiles and finish your task. Go for it,young soldier!");
  glcd.display();
  delay(6000);
  glcd.clear();*/
  pinMode(BL, INPUT_PULLUP);
  pinMode(BR, INPUT_PULLUP);
}

/*************************************** Loop **************************************************************/
#define MSNUM 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

void loop()                     
{
/*************************************** parameters that can be modified***********************************************/
  int csize=7; //the size of a char
  int dis=5;//the distance each move takes
  uint8_t missile[MSNUM][3];  
  int xpos = 64;//the position of the plane
  
  loopCount=0;
  uint8_t screw =0;
  
  String score;
  String highscore;
  int score_num;
  
  randomSeed(millis());     // seed with the current time


  // initialize
  for (uint8_t f=0; f< MSNUM; f++) {
    missile[f][XPOS] = random(122);
    missile[f][YPOS] = csize;
    missile[f][DELTAY] = random(2) + 1;
  }

/****************************************** start looping ***********************************************/
  //Serial.println("Start");
  while (1) {
    score = "Score:";
    highscore = " Highest:";
    
    Serial.println(score);
    Serial.println(highscore);
    score_num=loopCount/5;
    score+=score_num;
    highscore+= high;
    Serial.println(score);
    Serial.println(highscore);
    
    //score+=highscore;
    //score+=high;
    char charBuf[50];
    char charBuf2[50];
    score.toCharArray(charBuf, 50); 
    highscore.toCharArray(charBuf2, 50);
    strcat(charBuf,charBuf2);
    /*
    Serial.println("Start");
    Serial.println(score);
    Serial.println(highscore);
    Serial.println(charBuf);//testing the score
    */
    glcd.drawstring(0, 0, charBuf);
    glcd.drawline(0,csize,128,csize,BLACK);
    // draw each missile
    for (uint8_t f=0; f< MSNUM; f++) {
   
      drawMissile(missile[f][XPOS], missile[f][YPOS]);
    }
    //Serial.println("1");
    drawPlane(xpos);
    
    glcd.display();
    //delay(10);
    state2 = digitalRead(BR);
    delay(10);
    state1 = digitalRead(BL);
    
    if(state2 == HIGH){
        xpos+=dis;
        xpos = min(114,xpos);
        //Serial.println("R");
    }
    delay(10);
    
    if(state1 == HIGH){
        xpos-=dis;
        xpos=max(0,xpos);
        //Serial.println("L");
    }
    
    
    // then update missile
    for (uint8_t f=0; f< MSNUM; f++) {
      //drawMissile(missile[f][XPOS], missile[f][YPOS]);
      // move it
      missile[f][YPOS] += missile[f][DELTAY];
      
      // if its gone, reinit
      if (missile[f][YPOS] > 64) {
        missile[f][XPOS] = random(128);
        missile[f][YPOS] = csize;
        missile[f][DELTAY] = min(random(max(loopCount/50,3)),7) + 1;
      }
    }
    
    //Serial.println(freeRam());
    loopCount=loopCount+1;

    //checking if the missile hits the plane
    for (uint8_t f=0; f< MSNUM; f++) {
      if(missile[f][YPOS]>50){
        if(missile[f][XPOS]> xpos-3 && missile[f][XPOS]< xpos+12 ){
          screw =1 ;
          break;
        }
      }
      
     
    }
    if(screw==1){
      break;
    }
    glcd.clear();
  }
  glcd.clear();

  if(high<score_num){
    /*if(score_num>35){
      high=score_num;
      glcd.drawstring(0, 0, "Bravo, you have made it! You are now the highest score holder");
      glcd.display();
      delay(3000);
      glcd.clear();
      glcd.drawstring(0, 0, "Please enter your name: ");
      glcd.display();
      delay(3000);
      glcd.clear();
      glcd.drawstring(0, 0, "Just kidding. There's no way to enter your name. So I will just put my name here.    Highest score holder: Jeff - Forever");
      glcd.display();
      delay(7000);
      glcd.clear();
    }*/
      high=score_num;
      glcd.drawstring(0, 0, "Bravo, you've made it! You are now the highest score holder");
      glcd.display();
      delay(3000);
      glcd.clear();
  }
  else{
    glcd.drawstring( 0, 0, "Unfortunately, you were hit by a missile and crash.Don't give up, try again!");
    glcd.display();
    delay(3000);
    glcd.clear();
  }
}

#ifdef __AVR__
// this handy function will return the number of bytes currently free in RAM, great for debugging!   
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 
#endif


/*************************************** draw plane *********************************************************************/

void drawPlane(uint8_t xpos) {
  int ypos=64;
  int scale = 1;
  int num = 12; // num of points
  int points[num+1][2]=
  {
        {0,1},
        {1,0},
        {3,2},
        {4,1},
        {5,2},
        {7,0},
        {9,2},
        {10,1},
        {11,2},
        {13,0},
        {14,1},
        {7,8},
        {0,1}
  }; //coordinate of BR bomber
  for(int i =0; i< num; i++){
      glcd.drawline(scale*points[i][0]+xpos, ypos-scale*points[i][1], scale*points[i+1][0]+xpos, ypos-scale*points[i+1][1], BLACK);
  }
}

/*************************************** end of draw plane **************************************************************/

/*************************************** draw missile *********************************************************************/

void drawMissile(uint8_t xpos, uint8_t ypos) {
  //int ypos=0;
  int scale = 1;
  int det = 10; // length of the missile
  int num = 7; // num of points
  int points[num+1][2]=
  {
        {0,0},
        {6,0},
        {4,2},
        {4,det},
        {3,det+1},
        {2,det},
        {2,2},
        {0,0}
  }; //coordinate of B2 bomber
  for(int i =0; i< num; i++){
      glcd.drawline(xpos+points[i][0], points[i][1]+ypos, xpos+points[i+1][0], ypos+points[i+1][1], BLACK);
  }
  glcd.fillrect(xpos+2*scale, ypos , scale*2, det, BLACK);
}

/*************************************** end of draw missile **************************************************************/
