/*
Code by: Chris Thiele
Grobo Inc.
Sept/12/2014
*/

// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

#define DHTPIN 3     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11		// DHT 11 
//#define DHTTYPE DHT22		// DHT 22 (AM2302)
//#define DHTTYPE DHT21		// DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor



//OUTPUT DEFINE
int pump = D0;
int led[] = {D1, D2}; 
int valve[] = {D4, D5, D6};

//INPUT DEFINE
int luxSense = A0;
int waterLevelSense = D7;

//GLOBAL VARIABLES
double temp = 0;
double hum = 0;
double lux = 0.0;
double luxRaw = 0.0;
double growCubeID = 0.0;
int waterLevel = 0;

//DHT Setup
DHT dht(DHTPIN, DHTTYPE); //Digital Pin 3


// This routine runs only once upon reset
void setup() {
    
    int i = 0;
    
    //Serial Setup
    Serial.begin(9600);
    dht.begin();

    //Spark.core functions
    Spark.function("led",ledControl);
    Spark.function("valve",valveControl);
    
    //Spark.core variables
    Spark.variable("temp", &temp, DOUBLE);
    Spark.variable("hum", &hum, DOUBLE);
    Spark.variable("lux", &lux, DOUBLE);
    Spark.variable("water", &waterLevel, INT);
    Spark.variable("growCubeID", &growCubeID, DOUBLE);
  
    //Pin type and direction setup
    pinMode(luxSense, INPUT);
    pinMode(waterLevelSense, INPUT);
    pinMode (pump, OUTPUT);
    for (i = 0; i <= 1; i++) pinMode(led[i], OUTPUT);
    for (i = 0; i <= 2; i++) pinMode(valve[i], OUTPUT);
  
    //Initialize to the off state
    digitalWrite (pump, LOW);
    for (i = 0; i <= 1; i++) digitalWrite(led[i],LOW);
    for (i = 0; i <= 2; i++) digitalWrite(valve[i],LOW);
}



/**************************MAIN LOOP*****************************/
//Don't delay for more then 5 seconds
void loop() {
    hum = dht.getHumidity();
	temp = dht.getTempCelcius();
    luxRaw = analogRead(luxSense);
    //Return: 0 = dim, 1 = nominal, 2 = bright
    lux = ((luxRaw * 3.3)/4095);
    waterLevel = digitalRead(waterLevelSense);
}

  
/**************************API FUNCTIONS***************************/
  
int ledControl(String command)
{
   //find out the pin number and convert the ascii to integer
   int pinNumber = (command.charAt(1) - '0');
   //Sanity check to see if the pin numbers are within limits
   if (pinNumber < 1 || pinNumber > 3) return -1;
   
   toggle (pinNumber);
 
   return 1;
}

int valveControl(String command)
{
    
    int pinNumber = ((command.charAt(1) - '0') + 3);//offset by +3 because valves start at D4
    if (pinNumber < 4 || pinNumber > 6) return -1;
    
    int valveOnTime = 5000;
    
    toggle (pinNumber); //turn on the specified valve
    digitalWrite(pump, HIGH);
    
    delay (valveOnTime);
    
    toggle (pinNumber); //turn off the specified valve
    delay (800);
    digitalWrite(pump, LOW);
    
    return 1;
}

/*
int growCubeIDGet(String command)
{
    //
    return 0;
}*/


/**************************FUNCTIONS***********************************/
void toggle (int pinNumber)
{
    int state = 0;
    int currentState = digitalRead(pinNumber);
    if (currentState == 1) state = 0;
    else state = 1; 
    digitalWrite(pinNumber, state);
}