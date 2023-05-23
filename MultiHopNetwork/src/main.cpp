
#include "Arduino.h"
#include "RHMesh.h"
#include "SPI.h"
#include "RH_RF95.h"
#include "SSD1306.h"
#include <Wire.h>


#define SS 18
#define RST 14
#define DI0 26

RH_RF95 rf95(SS, DI0);

SSD1306 display(0x3c, 4, 15);



void display_print(String DisplayText, int column, int line)
{
  display.init();
  display.clear();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(column, line, DisplayText); 
  display.display();
}

void display_init()
{
  pinMode(16,OUTPUT);   
  digitalWrite(16, LOW);
  delay(50);
  digitalWrite(16, HIGH);
}

void setup()
{
    Serial.begin(9600);
    while(!Serial);

    display_init();

    if(!rf95.init())
    {
        Serial.println("init failed");
        display_print("init failed", 10, 10);
        
    }
}

void loop()
{
    //Serial.println("Sending");
    
    //display_print("Sending", 10, 10);
    
    uint8_t data[] = "Hello World!";
    //rf95.send(data, sizeof(data));
    //rf95.waitPacketSent();

    uint8_t buf[RH_RF95_MAX_PAYLOAD_LEN];
    uint8_t len = sizeof(buf);

    if(rf95.waitAvailableTimeout(3000))
    {
        if(rf95.recv(buf,&len))
        {
            Serial.println("got reply:");
            Serial.println((char*)buf);
            
            display_print((char*)buf, 10 , 10);
           
        } else 
        {
            Serial.println("recv failed");
            
            display_print("recv failed", 10, 10);
            
        }
    }
    else 
    {
        Serial.println("No Reply");
        
        display_print("No Reply", 10, 10);
       
    }
    
    delay(2000);
}


