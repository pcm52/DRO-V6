// =========================HEADER=============================================================
/*
   Arduino Uno/Nano/328p Hardware Setup
   
            Quadrature inputs from AS5311

            Quad A --------------------  D2
            Quad B --------------------  D3

            2.2" SSD1305 OLED Display on SPI bus
            
            SCK    --------------------  D13
            MOSI   --------------------  D11
            CS	   --------------------  D7
            D/C    --------------------  D6
            RST    --------------------  D8
            
            Units Switch --------------  D4
            Zero Switch ---------------  D5           
            		
//============================================================================================
*/
#include <SPI.h>
#include "U8g2lib.h"
#include <Encoder.h>

U8G2_SSD1305_128X32_ADAFRUIT_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 8);

// Slave Select pins for encoder 1
signed long encoder1count = 0;
float mm;
float inch;
volatile int Units = 0;                    // default value 0 = metric mm, 1 = inches
// Character Arrays for sending to Display
char Reading2[9];
char Reading3[9];
// Switch debounce counters
long lastDebounceTime1 = 0;
long lastDebounceTime2 = 0;
long oldPosition = -999;
long newPosition = 0;

Encoder Encoder1(2, 3);


void setup() {
  
 pinMode(4,INPUT);    // push button to toggle units
 pinMode(5,INPUT);    // push button to zero display count  
 u8g2.begin(); 
 
}

void loop() {
 
    // check pin 5                    // Zero display count
    if(digitalRead(5) == LOW){
       if ((millis() - lastDebounceTime1) > 200) {
        lastDebounceTime1 = millis();
        //oldPosition = -999;
        //newPosition = 0;
        Encoder1.readAndReset();
       }
    }

    // check pin 4                    // select Metric or Imperial
    if(digitalRead(4) == LOW){
       if ((millis() - lastDebounceTime2) > 200) {
        lastDebounceTime2 = millis();
        Units = !Units;  
      }
    }    
    newPosition = Encoder1.read();
    if (newPosition != oldPosition) {
        oldPosition = newPosition;
    }
    mm = (float)newPosition/512.000; 
    inch = (float)newPosition/13004.8; 
    // using the dtostrf function below since this sets the number of decimal places for either metric or imperial readings
    dtostrf(mm,7,3,Reading2);
    dtostrf(inch,8,4,Reading3);

    // Display Readings
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_fub14_tf);
    u8g2.drawStr(0,28,"Y");
    if (Units == 0) {
      //u8g2.setFont(u8g2_font_t0_12_tf);  
      u8g2.setFont(u8g2_font_fub14_tf);
      u8g2.drawStr( 24, 28, Reading2);
      u8g2.setFont(u8g2_font_t0_12_tf);
      u8g2.drawStr( 110, 28, "mm");
    }
    if (Units == 1) {
      u8g2.setFont(u8g2_font_fub14_tf);  
      u8g2.drawStr( 24, 28, Reading3);
      u8g2.setFont(u8g2_font_t0_12_tf);  
      u8g2.drawStr( 110, 28, "''"); 
    }
    u8g2.sendBuffer();
    delay(50);    
}


