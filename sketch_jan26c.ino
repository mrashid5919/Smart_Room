//* How to use the DHT-11 sensor with Arduino
//   Temperature and humidity sensor and
//   I2C LCD1602
 //  SDA --> A4
 //  SCL --> A5

//Libraries
#include <DHT.h>;
//I2C LCD:
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x3F,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
  
//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
//int chk;
int h;  //Stores humidity value
int t; //Stores temperature value

// LDR
int LDRInput=A0;

int LED=8;

// servo motor
#include <Servo.h> 
// Declare the Servo pin 
int servoPin = 9; 
Servo Servo1;     // servo obj

//IR sensor in
int SensorPin=2;
int inFlag=0;

//IR sensor out
int SensorPin2=5;
int outFlag=0;

//People Count
int peopleCount=0;
void setup()
{
    Serial.begin(9600);
    //Serial.println("Temperature and Humidity Sensor Test");
    dht.begin();
    lcd.init(); //initialize the lcd
    lcd.backlight(); //open the backlight
    pinMode(LDRInput,INPUT);
    pinMode(LED,OUTPUT);
    Servo1.attach(servoPin);    // We need to attach the servo to the used pin number 
    pinMode(SensorPin,INPUT);
    pinMode(SensorPin2,INPUT);
}

void loop()
{

  int value=analogRead(LDRInput);
  
    if(value<300)
    {
      digitalWrite(LED,HIGH);
    }
    else
    {
      digitalWrite(LED,LOW);
    }
      
    //Read data and store it to variables h (humidity) and t (temperature)
    // Reading temperature or humidity takes about 250 milliseconds!
    h = dht.readHumidity();
    t = dht.readTemperature();
    
    //Print temp and humidity values to serial monitor
    /*Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %, Temp: ");
    Serial.print(t);
    Serial.println(" ° Celsius");*/
        
// set the cursor to (0,0):
// print from 0 to 9:

    lcd.setCursor(0, 0);
    lcd.println(" Now Temperature ");
    
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(t);
    lcd.print("C");

    lcd.setCursor(6, 1);
    lcd.println("1.3pm ");
     
    lcd.setCursor(11, 1);
    lcd.print("H:");
    lcd.print(h);
    lcd.print("%");

// motor
   // Make servo go to 0 degrees 
   //Servo1.write(0); 
   //delay(1000); 
   // Make servo go to 90 degrees 
   //Servo1.write(90); 
   //delay(1000); 
   // Make servo go to 180 degrees 
   

//IR
  if(outFlag==0)
  {
     int SensorValue=digitalRead(SensorPin);
  if(SensorValue==LOW)
  {
      Servo1.write(0);   //open
      Serial.println("low");
      inFlag=1;
  }
  else
 {
      Servo1.write(120);  // close
      Serial.println("high");
      inFlag=0;
 }  
 delay(2000);
  }

//IR2
if(inFlag==0)
{
   int SensorValue2=digitalRead(SensorPin2);
  if(SensorValue2==LOW)
  {
      Servo1.write(0); //open
      Serial.println("open");
      outFlag=1;  
  }
  else
{
      Servo1.write(120); //close
      Serial.println("close");
      outFlag=0;
}    
  
 delay(2000);
}
      /*int SensorValue2=digitalRead(SensorPin2);
  if(SensorValue2==LOW)
  {
      Servo1.write(120);  
  }
  else
{
      Servo1.write(0);
}  
delay(1000);
*/
}