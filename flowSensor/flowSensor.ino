
// Include EEPROM-like API for FlashStorage
#include <FlashAsEEPROM.h>

//Includes for WIFI
#include <WiFi101.h>
#include <SPI.h>
#include <RTCZero.h>
RTCZero rtc; 

//variables for flow rate calculation
int flowPin = 7; //D7
int time = 0;
int val1 = 0;
int val2 = 0;
float freq = 0;
float avgFreq = 0;
int dailyWaterUsage = 0;


void setup() {
  pinMode(flowPin, INPUT);    //Initialize the pin reciving data from the flow sensor
  EEPROM.write(0,dailyWaterUsage);
  EEPROM.commit();
}

void loop(){
    //average the value over 5 samples
    for(int i = 0; i < 5; i++){
        while(freq == 0){
            time += 1;

            //waiting for the first pulse
            if((digitalRead(flowPin) == HIGH)&&(val1==0)){
                val1 = time;
                while(digitalRead(flowPin) == HIGH){time += 1;} //waits for the pulse to continue being HIGH
            }
            
            //waiting for the second pulse
            if((digitalRead(flowPin) == HIGH)&&(val1!=0)){
                val2 = time;
            }

            //calculates frequency
            if((val1!=0)&&(val2!=0)){
                freq = 1/(val2 - val1);
                avgFreq = avgFreq + freq;
            }
        }

        val1 = val2 = freq = 0;
    }

    //if there is a request from the OpenSprinkler,
    //send the avgFreq
    avgFreq = avgFreq/5;

    //increase the daily water usage 

    //code to store the daily water usage
    /*
        If the day is over,
            dailyWaterUsage = dailyWaterUsage + EEPROM.read(0);
            my_flash_store.write(dailyWaterUsage);
            EEPROM.commit();
            dailyWaterUsage = 0;

        If its the last day of the month,
            yearlyWaterUsage = yearlyWaterUsage + EEPROM.read(0);
            store yearlyWaterUsage in the EEPROM
            send an email with the value in EEPROM.read(0);
    
        If its the first day of the year,
            send an email with the value in EEPROM.read(1);
            yearlyWaterUsage = 0;
    */

}