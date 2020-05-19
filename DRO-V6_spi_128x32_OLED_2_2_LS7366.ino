// =========================HEADER=============================================================
/*
   Arduino Uno/Nano/328p
   
           LS7366  -------------------   Arduino
        ------------                    ---------
            MOSI   -------------------   SDO (D11)
            MISO   -------------------   SDI (D12)
            SCK    -------------------   SCK (D13)
            SS1    -------------------   SS1 (D10)  *********** Changed from previous version!!!!!!!!!********
            GND    -------------------   GND
            VDD    -------------------   VCC (5.0V)
            
             2.2" SSD1305/9 OLED Display on SPI bus
            
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

//U8G2_SSD1305_128X32_ADAFRUIT_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 8);
//U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 4); /* 
//U8G2_SSD1309_128X64_NONAME0_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 4); /* 
U8G2_SSD1309_128X64_NONAME2_F_4W_HW_SPI u8g2(U8G2_R0, 7, 6 ,8);
// 
// U8g2 Library supports many other Display types and fonts
// See U8g2 github site for more details
//
// Slave Select pins for encoder 1
const int slaveSelectEnc1 = 10;
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

void setup() {
  
 pinMode(4,INPUT);    // push button to toggle units
 pinMode(5,INPUT);    // push button to zero display count  
 initEncoders();  
 clearEncoderCount();
 u8g2.begin(); 
 
}

void loop() {
 
    // check pin 5                    // Zero display count
    if(digitalRead(5) == LOW){
       if ((millis() - lastDebounceTime1) > 200) {
        lastDebounceTime1 = millis();
        clearEncoderCount();
       }
    }

    // check pin 4                    // select Metric or Imperial
    if(digitalRead(4) == LOW){
       if ((millis() - lastDebounceTime2) > 200) {
        lastDebounceTime2 = millis();
        Units = !Units;  
      }
    }    
    // Retrieve current encoder counters
    encoder1count = readEncoder(1);
    mm = (float)encoder1count/512.000;
    inch = (float)encoder1count/13004.8;
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

void initEncoders() {
  
  // Set slave selects as outputs
  pinMode(slaveSelectEnc1, OUTPUT);
  
  // Raise select pins
  // Communication begins when you drop the individual select signsl
  digitalWrite(slaveSelectEnc1,HIGH);
  
  SPI.begin();
  
  // Initialize encoder 1
  //    Clock division factor: 0
  //    Negative index input
  //    free-running count mode
  //    x1 quatrature count mode (one count per quadrature cycle)
  //    Note on powerup, both MDR0 and MDR1 are set to 0
  digitalWrite(slaveSelectEnc1,LOW);        // Begin SPI communication
  SPI.transfer(0x88);                       // Write to MDR0
  SPI.transfer(0x03);                       // 4 count per quad cycle
  digitalWrite(slaveSelectEnc1,HIGH);       // Terminate SPI conversation 
  delay(100);                              

}

long readEncoder(int encoder) {
  
  // Initialize temporary variables for SPI read
  unsigned int count_1, count_2, count_3, count_4;
  long count_value;  
  
  if (encoder == 1) {
  
  // Read encoder 1
    digitalWrite(slaveSelectEnc1,LOW);      // Begin SPI 
    SPI.transfer(0x60);                     // Request count
    count_1 = SPI.transfer(0x00);           // Read highest order byte
    count_2 = SPI.transfer(0x00);           
    count_3 = SPI.transfer(0x00);           
    count_4 = SPI.transfer(0x00);           // Read lowest order byte
    digitalWrite(slaveSelectEnc1,HIGH);     // Terminate SPI conversation 
  
  // Calculate encoder count
  count_value = (count_1 << 8) + count_2;
  count_value = (count_value << 8) + count_3;
  count_value = (count_value << 8) + count_4;
  
  return count_value;
  }
}

void clearEncoderCount() {
    
  digitalWrite(slaveSelectEnc1,LOW);      // Begin SPI conversation  
  // CLR counter
  SPI.transfer(0x20);  // Select counter register and clear    
  digitalWrite(slaveSelectEnc1,HIGH);     // Terminate SPI conversation 
  delayMicroseconds(100); 
}
 

