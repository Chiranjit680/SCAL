#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// SoftwareSerial pins (GPIO numbers)
SoftwareSerial Arduino(1,3); // Connect RX to D1, TX to D2

// WiFi credentials
const char* ssid = "DE                                                                         SKTOP-DEI238V 0691";
const char* password = "12344321";

// ThingSpeak credentials
unsigned long channelID = 2564277; // Replace with your ThingSpeak channel ID
const char* writeAPIKey = "HH9A7JPCH6R857KO"; // Replace with your ThingSpeak Write API key

WiFiClient client;
HTTPClient http;

// URLs for GET requests
const char* url1 = "http://api.thingspeak.com/talkbacks/52472/commands/41849907.json?api_key=OZN6DFT0C2LJM6I9";//angle - 180 to 180
const char* url2 = "https://api.thingspeak.com/talkbacks/52541/commands/42075142.json?api_key=B0CS5OJZTBDXVK2B";//spark plug -1,0

String previousRx1 = ""; // To store the previous value  
String previousRx2 = "";

// Function to get command from URL1
String GETCommand1() {
    String result; // Variable to store the result
    if (WiFi.status() == WL_CONNECTED) { // Check if the WiFi is connected
        http.begin(client, url1); // Specify request destination
        int httpCode = http.GET();
        if (httpCode > 0) { // Check the returning status code from the server
            String response = http.getString(); // Get the response from the server
            int index_start = response.indexOf("command_string\":\"") + 17; // Find the index where the command starts
            if (index_start > 16) { // Check if the substring index is valid
                int index_end = response.indexOf("\"", index_start); // Find the end of the command string
                if (index_end > index_start) {
                    result = response.substring(index_start, index_end); // Extract the command string
                }
            }
        } else {
            Serial.println("HTTP GET failed, error: " + String(httpCode)); // Print error if connection failed
        }
        http.end(); // Close connection
    } else {
        Serial.println("Failed to connect to WiFi"); // WiFi is not connected
    }
    return result; // Return the result
}

// Function to get command from URL2
String GETCommand2() {
    String result; // Variable to store the result
    if (WiFi.status() == WL_CONNECTED) { // Check if the WiFi is connected
        http.begin(client, url2); // Specify request destination
        int httpCode = http.GET();
        if (httpCode > 0) { // Check the returning status code from the server
            String response = http.getString(); // Get the response from the server
            int index_start = response.indexOf("command_string\":\"") + 17; // Find the index where the command starts
            if (index_start > 16) { // Check if the substring index is valid
                int index_end = response.indexOf("\"", index_start); // Find the end of the command string
                if (index_end > index_start) {
                    result = response.substring(index_start, index_end); // Extract the command string
                }
            }
        } else {
            Serial.println("HTTP GET failed, error: " + String(httpCode)); // Print error if connection failed
        }
        http.end(); // Close connection
    } else {
        Serial.println("Failed to connect to WiFi"); // WiFi is not connected
    }
    return result; // Return the result
}

void setup() {
  Serial.begin(9600);
  Arduino.begin(4800);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {

    String cmd1 = GETCommand1(); 
   String cmd2 = GETCommand2();
     String result = "";

  // Check if there is a change in rx value
  if (cmd1 != previousRx1 || cmd2 != previousRx2) {
    result = cmd1 + "," + cmd2;
     Arduino.println("?CHANGED:" + result);
    previousRx1 = cmd1; 
    previousRx2 = cmd2;
  }
  // Check if there is new data from Arduino
  if (Arduino.available() > 0 ) {
    String data = Arduino.readStringUntil('\n');
     if (data.indexOf("#")==0)
       parseAndSendData(data);
  //  Serial.println(data);
    // Parse the data received from Arduino
   
  }

  // Delay for a while before the next iteration
  delay(1500);
}

// Function to parse and send data to ThingSpeak
void parseAndSendData(String data) {
  // Split the data using comma as delimiter
  int index1 = data.indexOf(',');
  int index2 = data.indexOf(',', index1 + 1);
  int index3 = data.indexOf(',', index2 + 1);
  int index4 = data.indexOf(',', index3 + 1);
  int index5 = data.indexOf(',', index4 + 1);

  if (index1 != -1 && index2 != -1 && index3 != -1 && index4 != -1 && index5 != -1) {
    String thermoCouple1 = data.substring(1, index1);
    String thermoCouple2 = data.substring(index1 + 1, index2);
    String mq2Value = data.substring(index2 + 1, index3);
    String mq7Value = data.substring(index3 + 1, index4);
    String flameSensor = data.substring(index4 + 1, index5);

    // Print the parsed values
    // Serial.println("Thermocouple 1: " + thermoCouple1);
    // Serial.println("Thermocouple 2: " + thermoCouple2);
    // Serial.println("MQ2 Value: " + mq2Value);
    // Serial.println("MQ7 Value: " + mq7Value);
    // Serial.println("Flame Sensor: " + flameSensor);

    // Send data to ThingSpeak
    ThingSpeak.setField(1, thermoCouple1);
    ThingSpeak.setField(2, thermoCouple2);
    ThingSpeak.setField(3, mq2Value);
    ThingSpeak.setField(4, mq7Value);
    ThingSpeak.setField(5, flameSensor);
     int channelID =2559973;
     const char* writeAPIKey="HF58EX1SNR180I7W";
    int x = ThingSpeak.writeFields(channelID,writeAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}
