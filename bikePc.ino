#include <Wire.h>  // Include Wire for using I2C for the miniOled
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

//////////////////////////
// MicroOLED Definition //
//////////////////////////
#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D
MicroOLED oled(PIN_RESET, DC_JUMPER);  // I2C Example


long magnet_nextWakeUp;
long reed_nextWakeUp;
long display_nextWakeUp;


const byte m_interruptPin = 14;
volatile byte m_interruptCounter = 0;
int m_numberOfInterrupts = 0;
float m_currentSpinDuration = 0.0; 

unsigned long m_lastToggleTimeReed = 0;  // the last time the output pin was toggled
unsigned long m_debounceDelay = 50;    // (in ms) the debounce time; increase if the output flickers

//set values
float m_wheelRadius = 37;               //radius of the wheel in cm 
float m_lowerSpeedDetectionLimit = 1.2; //(in km/h) if the speed is under this limit it's set to zero

//calculated values
float m_lengthWheel = 0;
float m_lowerSpeedTimeLimit = 0;        //(in ms) minimum time between two triggers until it's not zero

void setup() 
{
  Serial.begin(115200);

  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.clear(PAGE); // Clear the buffer.

  printTitle("start !", 1);

  
  pinMode(m_interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(m_interruptPin), reedContactInterrupt, FALLING);

  m_lengthWheel = lengthOfWheel(m_wheelRadius);

  float turnsPerSecond = (m_lowerSpeedDetectionLimit / 3.6) / m_lengthWheel;
  m_lowerSpeedTimeLimit = (1 / turnsPerSecond) * 1000;

   Serial.print("m_lowerSpeedTimeLimit in ms : ");
   Serial.println(m_lowerSpeedTimeLimit);
}
 
void loop() 
{
    //Serial.print("current spin duration in seconds: ");
    //Serial.println(m_currentSpinDuration/1000);
    //Serial.println("");

    float kilometersPerHour = timeToKilometerPerHour(m_currentSpinDuration);

    Serial.print(kilometersPerHour);
    Serial.print(" millis: ");
    Serial.println(m_currentSpinDuration);

    printTitle(String(kilometersPerHour, 2), 2);

    if(millis() - m_lastToggleTimeReed > m_lowerSpeedTimeLimit)
    {
      m_currentSpinDuration = 0;
    }


  delay(100);
    
}

void reedContactInterrupt() 
{
  unsigned long currentMillis = millis();
  
  if ((currentMillis - m_lastToggleTimeReed) > m_debounceDelay)
  {
    //last reading was longer ago than the debounce delay
    
    Serial.println("interrupt triggered, after debounce time");

    m_currentSpinDuration = currentMillis - m_lastToggleTimeReed;
    
    m_lastToggleTimeReed = currentMillis;
  }
  else
  {
     Serial.println("FLICKER: interrupt triggered, but during debounce time!");
  }  
}

float lengthOfWheel(float m_wheelRadius)
{
  float length = (3.14159265 * pow((m_wheelRadius/ 100), 2)) ;
  Serial.print("lenght of wheel in meter: ");
  Serial.println(length);

  return length;
}

float timeToKilometerPerHour(float rotationSpeedMillis)
{
  if(rotationSpeedMillis == 0)
  {
    return 0;
  }
  else
  {  
    return ( (1/(rotationSpeedMillis/1000)) * m_lengthWheel) * 3.6 ;
  }
}

//oled-stuff
// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
void printTitle(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;
  
  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  //oled.setCursor(middleX - (oled.getFontWidth() * (title.length()/2)),
  //               middleY - (oled.getFontWidth() / 2));

  // Try to set the cursor in the middle(y) and left of the screen
  oled.setCursor(0, middleY - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
  delay(600);
  oled.clear(PAGE);
}


