const byte m_interruptPin = 14;
volatile byte m_interruptCounter = 0;
int m_numberOfInterrupts = 0;
float m_currentSpinDuration = 0.0; 

unsigned long m_lastToggleTimeReed = 0;  // the last time the output pin was toggled
unsigned long m_debounceDelay = 50;    // (in ms) the debounce time; increase if the output flickers

//set values
float m_wheelRadius = 60;               //radius of the wheel in cm 
float m_lowerSpeedDetectionLimit = 1.2; //(in km/h) if the speed is under this limit it's set to zero

//calculated values
float m_lengthWheel = 0;
float m_lowerSpeedTimeLimit = 0;        //(in ms) minimum time between two triggers until it's not zero

void setup() 
{
  Serial.begin(115200);
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

    Serial.print(timeToKilometerPerHour(m_currentSpinDuration));
    Serial.print(" millis: ");
    Serial.println(m_currentSpinDuration);

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


