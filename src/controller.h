#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <Adafruit_MCP3008.h>

#define sens_cnt 8

//const int line_value[sens_cnt] = {172,220,193,141,238,185,230,259};
const int line_value[sens_cnt] = {984,986,984,980,984,982,984,983};//white line
//#define lineBlack true
const int maxdiff = 100;

#define M_A 3
#define M_B 5
#define M_C 6
#define M_D 9
#define LED1 A0
#define LED2 A1
#define LED3 A2
#define LED4 A3
#define button 2
#define ser A0
void(* resetFunc) (void) = 0; //declare reset function @ address 0

class Sunsheild{
    private:
        SparkFun_APDS9960 apds = SparkFun_APDS9960();
        uint16_t ambient_light = 0;
        uint16_t red_light = 0;
        uint16_t green_light = 0;
        uint16_t blue_light = 0;
        Adafruit_MCP3008 adc;
        int thereshold [sens_cnt];
    public:
        void init(){
            for(int i=0;i<sens_cnt;i++){
                #ifdef lineBlack
                    thereshold[i] = line_value[i]+maxdiff;
                #else
                    thereshold[i] = line_value[i]-maxdiff;
                #endif
            }

            pinMode(M_A,OUTPUT);
            pinMode(M_B,OUTPUT);
            pinMode(M_C,OUTPUT);
            pinMode(M_D,OUTPUT);
           // pinMode(LED,OUTPUT);

            pinMode(LED1,OUTPUT);
            pinMode(LED2,OUTPUT);
            pinMode(LED3,OUTPUT);
            pinMode(LED4,OUTPUT);
            digitalWrite(LED1,HIGH);
            digitalWrite(LED2,HIGH);
            digitalWrite(LED3,HIGH);
            digitalWrite(LED4,HIGH);

            //myservo.attach(ser);

            adc.begin();
            
            apds.init();
            apds.enableLightSensor(false);
            apds.setMode(DEFAULT_CONFIG3,1);
            delay(500);
        }
        void stop(){
            digitalWrite(M_A,LOW);
            digitalWrite(M_B,LOW);
            digitalWrite(M_C,LOW);
            digitalWrite(M_D,LOW);
        }
        void ledON(int LED){
            digitalWrite(LED,LOW);
        }
        void ledOFF(int LED){
            digitalWrite(LED,HIGH);
        }
        void waitButton(){
            while (digitalRead(button) == LOW);
        }
        void rstIfPressed(){
            /*
            if(digitalRead(button) == HIGH){
            resetFunc();
            }*/
        }
        void right (int16_t speed)//motor
        {
            if(speed > 0){
                analogWrite(M_A,speed);
                analogWrite(M_B,0);   
            }
            else{
                analogWrite(M_A,0);
                analogWrite(M_B,-speed);
            }
        }
        void left (int16_t speed)
        {
            if(speed > 0){
                analogWrite(M_C,speed);
                analogWrite(M_D,0);   
            }
            else{
                analogWrite(M_C,0);
                analogWrite(M_D,-speed);
            }
        }
    
        bool grabColor(uint16_t * r,uint16_t * g,uint16_t * b){
            if (apds.readRedLight(red_light) && apds.readGreenLight(green_light) && apds.readBlueLight(blue_light) ) {
                *r = red_light;
                *g = green_light;
                *b = blue_light;
                return true;
            } else {
                return false;
            }
        }
        int getVal(uint8_t chan){
            return adc.readADC(7-chan);
        }
        bool isLine(uint8_t chan){
            int val = adc.readADC(7-chan);  //left to right 0-7
            #ifdef lineBlack
                return val < thereshold[chan];
            #else 
                return val > thereshold[chan];
            #endif
        }
        bool isLine_LEFT(uint8_t chan_cnt){
            for(uint8_t c=0;c<chan_cnt;c++){
                if(this->isLine(c))
                    return true;
            }
            return false;
        }
        bool isLine_RIGHT(uint8_t chan_cnt){
            for(int8_t c=chan_cnt-1;c>=0;c--){
                if(this->isLine(c))
                    return true;
            }
            return false;
        }
        bool isLine_MID50(){
            return this->isLine(3) || this->isLine(4);
        }
        bool isLine_MID100(){
            return this->isLine(3) && this->isLine(4);
        }
        void flagUp(){
            for(int i = 0;i<10;i++){
                digitalWrite(ser,HIGH);
                delay(1);
                digitalWrite(ser,LOW);
                delay(19);
            }
        }
        void flagDown(){
            for(int i = 0;i<10;i++){
                digitalWrite(ser,HIGH);
                delay(2);
                digitalWrite(ser,LOW);
                delay(18);
            }
        }
};
int start = 0;
void timerStart(){
    start = millis();
}
unsigned int timerStop(){
    return millis()-start;
}