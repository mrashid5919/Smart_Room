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
int fan = 11;       // the pin where fan is 
int tempMin = 31;   // the temperature to start the fan
int tempMax = 34;   // the maximum temperature when fan is at 100%
int fanSpeed;
int fanLCD;
int baseSpeed=60;

boolean peopleOnBothSide=false;
long int door_move_starting_time;
int door_move_angle;

// LDR
int LDRInput=A0;

int LED=8;

// servo motor
#include <Servo.h> 
// Declare the Servo pin 
int servoPin = 9; 
Servo Servo1;     // servo obj

//IR sensor outside
int irPinOUTSIDE=2;

//IR sensor inside
int irPinINSIDE=5;

int count=0;
boolean init_state=true;
boolean wait_enter_state = false;
boolean wait_exit_state = false;
long int waiting_time;

char data=0;
int bluetoothControl=0;
boolean ledOn=false;
boolean fanOn=false;

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
    pinMode(irPinOUTSIDE,INPUT);
    pinMode(irPinINSIDE,INPUT);
    pinMode(fan, OUTPUT);
}

void LEDLogic()
{
  if(count && bluetoothControl==0){
int value=analogRead(LDRInput);
  
    if(value<300)
    {
      digitalWrite(LED,HIGH);
    }
    else
    {
      digitalWrite(LED,LOW);
    }
  }
  else
    digitalWrite(LED,LOW);
}

void doorLogic(){
    int inputINSIDE = !digitalRead(irPinINSIDE);
    int inputOUTSIDE = !digitalRead(irPinOUTSIDE);
    //this function can change a state variable

    if(millis() - door_move_starting_time<3000){
        Servo1.write(door_move_angle);
    }
    else{
        if(inputINSIDE && inputOUTSIDE){
            Servo1.write(90);
            peopleOnBothSide=true;
        }
        else if(!inputINSIDE && !inputOUTSIDE){
          Servo1.write(90);
          peopleOnBothSide=false;
        }
        else if(inputINSIDE && !inputOUTSIDE){
          Servo1.write(0);
          door_move_starting_time = millis();
          door_move_angle=0;
          peopleOnBothSide=false;
        }
        else{
          Servo1.write(0);
          door_move_starting_time = millis();
          door_move_angle=0;
          peopleOnBothSide=false;
        }
    }
    
    
}

void humanCount(){
  int inputINSIDE = !digitalRead(irPinINSIDE);
  int inputOUTSIDE = !digitalRead(irPinOUTSIDE);
  delay(100);
  if(init_state){
      if(inputINSIDE && inputOUTSIDE){

      }
      else if(!inputINSIDE && !inputOUTSIDE){
          
      }
      else if(inputINSIDE && !inputOUTSIDE){
        init_state = false;
        wait_exit_state=true;
        waiting_time = millis();
      }
      else{
        init_state = false;
        wait_enter_state=true;
        waiting_time = millis();
      }
  }
  else if(wait_enter_state){
      if(inputINSIDE && inputOUTSIDE){
        init_state=true;
        wait_enter_state=false;
      }
      else if(!inputINSIDE && !inputOUTSIDE){
        if(millis()-waiting_time>3000){
          init_state=true;
          wait_enter_state=false;
        }
      }
      else if(inputINSIDE && !inputOUTSIDE){
        init_state = true;
        wait_enter_state=false;
        count++;
        Serial.print("Entering : ");
        Serial.println(count);
      }
      else{
        
      }
  }
  else{
      if(inputINSIDE && inputOUTSIDE){
        init_state=true;
        wait_exit_state=false;
      }
      else if(!inputINSIDE && !inputOUTSIDE){
        if(millis()-waiting_time>3000){
          init_state=true;
          wait_exit_state=false;
        }
      }
      else if(inputINSIDE && !inputOUTSIDE){
        
      }
      else{
        init_state = true;
        wait_exit_state=false;
        if(count>0) count--;
        Serial.print("Exiting : ");
        Serial.println(count);
      }
  }
}



void fanSpeedLogic(int temp){
  //humidity
  if(count && bluetoothControl==0){
    if((temp >= tempMin) && (temp <= tempMax)) {  // if temperature is higher than minimum temp
         fanSpeed = baseSpeed+(temp-tempMin)*10; 
         analogWrite(fan, fanSpeed);  // spin the fan at the fanSpeed speed
     } 
    
     if(temp < tempMin) {   // if temp is lower than minimum temp
      fanSpeed = 0;      // fan is not spinning
      digitalWrite(fan, LOW);       
     } 
     
     if(temp > tempMax) {        // if temp is higher than tempMax
      fanSpeed = 100;      // fan is spinning
      //fanLCD = 255; 
      digitalWrite(fan, HIGH); 
     }
  }
  else{
    digitalWrite(fan,LOW);
  }
}

void bluetoothLogic()
{
    if(count)
    {
        if(Serial.available()>0)
        {
            data = Serial.read();   
            if(bluetoothControl==1)
            {
                  if(data == 'a')   {          
                      digitalWrite(LED, HIGH);
                      ledOn=true;  
                  }
                  else if(data == 'b') {    
                      digitalWrite(LED, LOW); 
                      ledOn=false;
                  }

                  if(data == 'c')   {          
                     digitalWrite(fan, HIGH);
                     fanSpeed=100;
                     fanOn=true; 
                  } 
                  else if(data == 'd')  {   
                      digitalWrite(fan, LOW);
                      fanSpeed=0;
                      fanOn=false;
                  }

            }
            
                  if(data == 'g') { 
                      bluetoothControl=1; 
                      Serial.println("g");
                  }            
                  else if(data == 'h')  {   
                      bluetoothControl=0;
                      Serial.println("h");
                  }
        }
    }
    
}

void loop()
{

  /*int value=analogRead(LDRInput);
  
    if(value<300)
    {
      digitalWrite(LED,HIGH);
    }
    else
    {
      digitalWrite(LED,LOW);
    }*/
    humanCount();
    LEDLogic();
    doorLogic();
    bluetoothLogic();
    if(bluetoothControl)
    {
        if(ledOn)
          digitalWrite(LED,HIGH);
        else
          digitalWrite(LED,LOW);
        if(fanOn){
          fanSpeed=100;
          digitalWrite(fan,HIGH);
        }
        else{
          fanSpeed=0;
          digitalWrite(fan,LOW);
        }
    }
      
    //Read data and store it to variables h (humidity) and t (temperature)
    // Reading temperature or humidity takes about 250 milliseconds!
    h = dht.readHumidity();
    t = dht.readTemperature();

    fanSpeedLogic(t);
    /*Serial.print("Init_state:");
    Serial.print(init_state);
    Serial.print("\nWait_exit_state:");
    Serial.print(wait_exit_state);
    Serial.print("\nWait_enter_state:");
    Serial.print(wait_enter_state);*/
    
    //Print temp and humidity values to serial monitor
    /*Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %, Temp: ");
    Serial.print(t);
    Serial.println(" ° Celsius");*/
        
// set the cursor to (0,0):
// print from 0 to 9:

    lcd.setCursor(0, 0);
    lcd.print(" Human: ");
    lcd.print(count);
    lcd.setCursor(10,0);
    lcd.print("F: ");
    if(fanSpeed==100)
      lcd.print(fanSpeed);
    else{
      lcd.print(" ");
      lcd.print(fanSpeed);
    }
    lcd.setCursor(0, 1);
    lcd.print(" T:");
    lcd.print(t);
    lcd.print("C");

     
    lcd.setCursor(10, 1);
    lcd.print("H:");
    lcd.print(h);
    lcd.print("%");
}
