#include <Timer5.h>

//variables for flow rate calculation
int flowPin = 19; //A5
int val1 = 0;
int val2 = 0;
float freq = 0;
float avgFreq = 0;
int dailyWaterUsage = 0;

//variables for timer 

int led = 13; //red LED
volatile int count=0;

long t = millis(); 


void setup() {
  pinMode(flowPin, INPUT);    //Initialize the pin reciving data from the flow sensor
  pinMode(led,OUTPUT);
  //timer set up
  // debug output at 115200 baud
	Serial.begin(115200);
	//while (!SerialUSB) ;
		
	Serial.println("starting");

    // define frequency of interrupt
	MyTimer5.begin(1);  // 200=for toggle every 5msec

    // define the interrupt callback function
  MyTimer5.attachInterrupt(Timer5_IRQ);
  
    // start the timer
  MyTimer5.start();
}

void loop(){
    //average the value over 5 samples
    for(int i = 0; i < 5; i++){
        while(freq == 0){

            //waiting for the first pulse
            if((digitalRead(flowPin) == HIGH)&&(val1==0)){
                val1 = count;
                while(digitalRead(flowPin) == HIGH){} //waits for the pulse to continue being HIGH
            }
            
            //waiting for the second pulse
            if((digitalRead(flowPin) == HIGH)&&(val1!=0)){
                val2 = count;
            }

            //calculates frequency
            if((val1!=0)&&(val2!=0)){
                freq = 1/(val2 - val1);
                freq = freq/2;
                avgFreq = avgFreq + freq;
            }
        }

        val1 = val2 = freq = 0;
    }

    //if there is a request from the OpenSprinkler,
    //send the avgFreq
    avgFreq = avgFreq/5;
    Serial.println(avgFreq);

}

// will be called by the MyTimer5 object
void Timer5_IRQ(void) {
    static bool on = false;
    count++;  // count number of toggles
    if (on == true) {
      on = false;
        digitalWrite(led,LOW);
    } else {
      on = true;
        digitalWrite(led,HIGH);
    }
}
