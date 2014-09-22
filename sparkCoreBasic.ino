/*
Code by: Chris Thiele
Grobo Inc.
Sept/12/2014
*/

// This #include statement was automatically added by the Spark IDE.
#include "MCP23008-I2C/MCP23008-I2C.h"

// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

#define DHTPIN 5     // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11		// DHT 11 
//#define DHTTYPE DHT22		// DHT 22 (AM2302)
//#define DHTTYPE DHT21		// DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Connect pin #1 of the expander to D1 (i2c clock)
// Connect pin #1 to 5V (using a pullup resistor ~4K7 (4700) ohms)
// Connect pin #2 of the expander to D0 (i2c data)
// Connect pin #2 to 5V (using a pullup resistor ~4K7 (4700) ohms)
// Connect pins #3, 4 and 5 of the expander to ground (address selection)
// Connect pin #6 and 18 of the expander to 5V (power and reset disable)
// Connect pin #9 of the expander to ground (common ground)

// Input #0 is on pin 10 so connect a button or switch from there to ground




//OUTPUT DEFINE
int pump = D2;
//int led[] = {D1, D2}; 
//int valve[] = {D3, D4};

//INPUT DEFINE
int luxSense = A0;
int waterLevelSense = D7;

//GLOBAL VARIABLES
double temp = 0;
double hum = 0;
double lux = 0.0;
double luxRaw = 0.0;
//double growCubeID = 0.0;
int waterLevel = 0;
static int state0 = 1;
static int state1 = 0;
static int state2 = 1;
static int state3 = 0;

//DHT Setup
DHT dht(DHTPIN, DHTTYPE); //Digital Pin 3
Adafruit_MCP23008 mcp;
Adafruit_MCP23008 mcp1;


// This routine runs only once upon reset
void setup() {
    
    int i = 0;
    
    //Serial Setup
    //Serial.begin(9600);
    dht.begin();
    mcp.begin(0);      // use default address 0
    mcp1.begin(7);      // use hard address 7
    

    //Spark.core functions
    Spark.function("led",ledControl);
    Spark.function("valve",valveControl);
    Spark.function("lightOff", lightOffControl);
    Spark.function("lightOn", lightOnControl);
    
    //Spark.core variables
    Spark.variable("temp", &temp, DOUBLE);
    Spark.variable("hum", &hum, DOUBLE);
    Spark.variable("lux", &lux, DOUBLE);
    Spark.variable("water", &waterLevel, INT);
    //Spark.variable("growCubeID", &growCubeID, DOUBLE);
  
    //Pin type and direction setup
    pinMode(luxSense, INPUT);
    pinMode(waterLevelSense, INPUT);
    pinMode (pump, OUTPUT);
    //for (i = 0; i <= 1; i++) pinMode(led[i], OUTPUT);
    //for (i = 0; i <= 1; i++) pinMode(valve[i], OUTPUT);
    for (i = 0; i <= 6; i++)
    {
        mcp.pinMode(i, OUTPUT);
        mcp1.pinMode(i, OUTPUT);
    }
  
    //Initialize to the off state
    digitalWrite (pump, LOW);
    //for (i = 0; i <= 1; i++) digitalWrite(led[i],LOW);
    //for (i = 0; i <= 1; i++) digitalWrite(valve[i],LOW);
    for (i = 0; i <= 6; i++)
    {
        mcp.digitalWrite(i, HIGH);
        mcp1.digitalWrite(i, HIGH);
    }
    
    mcp.digitalWrite(3, LOW);
    mcp1.digitalWrite(3, LOW);
    
    //Initialize addressed I2C busses
    mcp1.digitalWrite(6, LOW);
    mcp.digitalWrite(5, LOW);
}

/*mcp Lookup table
0 - RGB Grow Light
1 - RGB Grow Light
2 - RGB Grow Light
3 - Valve
4 - RGB Indication LED
5 - RGB Indication LED
6 - RGB Indication LED
7 - Unpopulated
*/

/**************************MAIN LOOP*****************************/
//Don't delay for more then 5 seconds
void loop() {
    hum = dht.getHumidity();
    delay(1000); //DHT11 has a physical read time of 1+ seconds, delay for 2 seconds
	temp = dht.getTempCelcius();
	delay(1000); //DHT11 has a physical read time of 1+ seconds, delay for 2 seconds
    lux = analogRead(luxSense); //luxRaw
    //Return: 0 = dim, 1 = nominal, 2 = bright
    //lux = ((luxRaw * 3.3)/4095);
    waterLevel = digitalRead(waterLevelSense);
    
}

  
/**************************API FUNCTIONS***************************/
  
  
int ledControl(String command)
{
   //find out the pin number and convert the ascii to integer
   /*int pinNumber = (command.charAt(1) - '0');
   //Sanity check to see if the pin numbers are within limits
   if (pinNumber < 1 || pinNumber > 2) return -1;
   
   toggle (pinNumber - 1, 0, 0); //pinNumber-1 refers to which I2C you're addressing, and 0 refers to the pinout position on that chip,
                                //DeviceType = LED
                       */         
    toggle (0, 0, 0); //just toggle both
    toggle (1, 0, 0);
 
   return 1;
}

int lightOnControl(String command)
{
    mcp.digitalWrite(0, LOW); //force both into high state
    mcp1.digitalWrite(0, LOW);
    
    return 1;
}

int lightOffControl(String command)
{
    mcp.digitalWrite(0, HIGH); //force both into low state
    mcp1.digitalWrite(0, HIGH);
    
    return 1;
}

int valveControl(String command)
{
    
    int pinNumber = (command.charAt(1) - '0');
    if (pinNumber < 1 || pinNumber > 2) return -1;
    
    int valveOnTime = 5000;
    
    toggle (pinNumber - 1, 3, 1); //turn on the specified valve, //offset by +2 because valves start at D3
    digitalWrite(pump, HIGH);
    
    delay (valveOnTime);
    
    toggle (pinNumber - 1, 3, 1); //turn off the specified valve
    delay (800);
    digitalWrite(pump, LOW);
    
    return 1;
}


/**************************FUNCTIONS***********************************/
void toggle (int growCube, int pinNumber, int deviceType)
{
    if (growCube == 0 && deviceType == 0) 
    {
        if (state0 == 1) state0 = 0;
        else state0 = 1;
        mcp.digitalWrite(pinNumber, state0);
    }
    else if (growCube == 0 && deviceType == 1) 
    {
        if (state1 == 1) state1 = 0;
        else state1 = 1;
        mcp.digitalWrite(pinNumber, state1);
    }
    else if (growCube == 1 && deviceType == 0) 
    {
        if (state2 == 1) state2 = 0;
        else state2 = 1;
        mcp1.digitalWrite(pinNumber, state2);
    }
    else //if (growCube == 1 && deviceType == 1) 
    {
        if (state3 == 1) state3 = 0;
        else state3 = 1;
        mcp1.digitalWrite(pinNumber, state3);
    }
}