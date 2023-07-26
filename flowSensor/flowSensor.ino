#define LED_PIN 13

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024

void startTimer(int frequencyHz);
void setTimerFrequency(int frequencyHz);
void TC3_Handler();

bool isLEDOn = false;

//variables for flow rate calculation
int flowPin = 19; //A5
int val1 = 0;
int val2 = 0;
float freq = 0;
float avgFreq = 0;
int dailyWaterUsage = 0;
volatile int count=0;


//adopted flow sensor code
volatile int flow_frequency; // Measures flow sensor pulsesunsigned 

int l_min; // Calculated litres/hour
unsigned char flowsensor = 19; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

void flow (){ // Interrupt function
   flow_frequency++;
}


void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(A5), flow, RISING); // pin, ISR, mode
  //sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
}

void loop() {
  currentTime = millis();// Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 5.05Q, Q is flow rate in L/min.
      l_min = (flow_frequency  / 5.05); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      flow_frequency = 0; // Reset Counter
      Serial.print(l_min, DEC); // Print litres/hour
      Serial.println(" L/min");
   }
}


