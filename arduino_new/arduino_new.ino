//this code reads thermocouple's value,flame sensor's value sets current angle,
#include <SoftwareSerial.h>
#include <Servo.h>
#include "max6675.h" // max6675.h file is part of the library that you should download from Robojax.com

Servo myservo;
SoftwareSerial Arduino(13, 12);

int flameSensorPin = 2; // Digital pin connected to the flame sensor
int flameState = 0;

int soPin1 = 4;// SO=Serial Out
int csPin1 = 5;// CS = chip select CS pin
int sckPin1 = 6;// SCK = Serial Clock pin
MAX6675 robojax1(sckPin1, csPin1, soPin1);// create instance object of MAX6675 1

int soPin2 = 7;// SO=Serial Out
int csPin2 = 8;// CS = chip select CS pin
int sckPin2 = 9;// SCK = Serial Clock pin
MAX6675 robojax2(sckPin2, csPin2, soPin2);// create instance object of MAX6675



const int analogPinMQ2 = A0; // Analog pin connected to MQ2 sensor
const int analogPinMQ7 = A1; // Analog pin connected to MQ7 sensor

const int sparkPlugPin = 10;
int sparkPlugValue =0;



void setup() {

          
  Serial.begin(9600);// initialize serial monitor with 9600 baud
  pinMode(flameSensorPin, INPUT);
  pinMode(sparkPlugPin,OUTPUT);
   myservo.attach(9);

  Arduino.begin(9600);

 

}

void loop() {
     sparkPlugValue=0;
  // data values in the form of thermocouple 1,thermocouple 2 ,MQ-2 ,MQ-7 ,flame_sensor
  // basic readout test, just print the current temp
  // Robojax.com MAX6675 Temperature reading on Serial monitor
//    Serial.println("Robojax MAX6675"); 
  flameState = digitalRead(flameSensorPin);
    int analogPinSensorValueMQ2 = analogRead(analogPinMQ2); // Read the analog value from sensor
    int analogPinSensorValueMQ7 = analogRead(analogPinMQ7); // Read the analog value from sensor
//  delay(500); // Wait for a second before reading again
  // Serial.print("C = "); 
     Arduino.print("#");
     Arduino.print(robojax1.readCelsius());
     Arduino.print(",");
     Arduino.print(robojax2.readCelsius());
     Arduino.print(",");
     Arduino.print(analogPinSensorValueMQ2); // Send the flame state to the   Arduino port
     Arduino.print(",");
     Arduino.print(analogPinSensorValueMQ7); // Send the flame state to the   Arduino port
     Arduino.print(",");
     Arduino.print(!flameState); // Send the flame state to the   NodeMCU port
     Arduino.println(",");
     delay(1000);

    if (Arduino.available() > 0) {  //how will ard. differentiate between its output and nOdeMCU's
    String input = Arduino.readStringUntil('\n');
        if (data.indexOf("?")==0)
        {
      
    //  Arduino.println(input);
    String extracted = extractStringAfterChanged(input);
     // Arduino.println(extracted);
      }


    //  Serial.print(robojax1.readCelsius());
    // Serial.print(",");
    // Serial.print(robojax2.readCelsius());
    // Serial.print(",");
    // Serial.println(analogPinSensorValueMQ2); // Send the flame state to the serial port
    // Serial.print(",");
    // Serial.println(analogPinSensorValueMQ7); // Send the flame state to the serial port
    // Serial.print(",");
    // Serial.println(!flameState); // Send the flame state to the serial port
    // Serial.print(",");
    //  delay(1000);

    // if (ArduinoUno.available() > 0) {
    // String input = ArduinoUno.readStringUntil('\n');
    // Serial.println(input);
    // String extracted = extractStringAfterChanged(input);
    // Serial.println(extracted);

    if (extracted != "") {
      int commaIndex =extracted.indexOf(',');
      int angle = extracted.substring(0,commaIndex).toInt();
    
       sparkPlugValue=extracted.substring(commaIndex+1).toInt();
      myservo.write(angle);

      if(sparkPlugValue==1)
      {
          digitalWrite(sparkPlugPin,HIGH);
          delay(2000);
          digitalWrite(sparkPlugPin,LOW);     

      }

    }
  }
}

String extractStringAfterChanged(const String& input) {
  const String keyword = "?CHANGED:";
  int pos = input.indexOf(keyword);
  if (pos != -1) {
    return input.substring(pos + keyword.length());
  }
  return "";
}







