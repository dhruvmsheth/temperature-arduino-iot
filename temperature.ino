/* This is the main.ino code for detection of person on entrance gate of the Supermarket or a mall
The Ultrasonic sensor is attached to the gate or entrance or exit of the mall and similarly it detects
the entry of a person and similarly, when a person is detetcted, it sends a command to the amg8833 temperature
module to read data (temperature) and this temperature is sent to ThingSpeak IoT cloud dashboard to view a 
graph or average temperature of the people inside the store. Using this, the visitors can view the average temperature 
of the people of the mall and in areas where abnormally high temperatures are dtected, alerts can be sent 
viewing the timestamps of these high temperatures and diagnosing people who visited in that time range
This model is deployed on the Arduino MKR WiFi 1010 - for IoT dashboard
The following code has been created by Dhruv Sheth for Arduino touch-free hackathon
cc @ dhruv sheth
those who wish to use this solution = licence - gpl3+

*/
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <Servo.h>
#include ESP32_Servo.h
#include <WiFi.h>
#include <WiFiMulti.h>
Servo servo1;
int trigPin = 9;
int echoPin = 8;
long distance;
long duration;

WiFiMulti WiFiMulti;

const char* ssid     = "JioFiber-tcz5n"; // Your SSID (Name of your WiFi) - This is a dummy name, enter your wifi ssid here
const char* password = "**********"; //I have not mentioned the password here, while running the cript, you may mention your pwd

const char* host = "api.thingspeak.com";
String api_key = "APIKEY"; // Your API Key provied by thingspeak


Adafruit_AMG88xx amg;

void setup()
{
 Serial.begin(9600);
servo1.attach(7); 
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);// put your setup code here, to run once:
}



{
    Connect_to_Wifi();
    Serial.println(F("AMG88xx test"));

    bool status;
    
    // default settings
    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }
    
    Serial.println("-- Thermistor Test --");

    Serial.println();

    delay(100); // let sensor boot up
}


void loop() 
ultra();
  servo1.write(0);
  if(distance <= 20){
   Serial.print("Thermistor Temperature = ");
    Serial.print(amg.readThermistor());
    Serial.println(" *C");
  
    Serial.println();
   // call function to send data to Thingspeak
   Send_Data();
    //delay
    delay(50);
    
  if(amg.readThermistor() > 38.1) // if a person with fever is detetcted, he is not allowed to enter
                                    // a person with fever has an avg body temperature of 38.1degree celsius
   servo1.write(180);
   digitalWrite(5, HIGH); //Turns on the buzzer to alarm people
  }
  
  void Connect_to_Wifi()
{

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(10);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}


 
void ultra(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  }


void Send_Data()
{

  Serial.println("Prepare to send data");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  else
  {
    String data_to_send = api_key;
    data_to_send += "&field1=";
    data_to_send += string(amg.readThermistor());
    data_to_send += "\r\n\";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + api_key + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data_to_send.length());
    client.print("\n\n");
    client.print(data_to_send);

    delay(200);  // reduced delay to perform real time data collection
  }

  client.stop();

}
