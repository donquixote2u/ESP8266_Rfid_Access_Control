/* if RF reader detects an authorised tag, fire door control solenoid
 *  RF reader outputs ID of tag read onto serial port
 * ESP8266 uses D7,D8  for rf reader alt serial port rx and debug tx, 
 *  D1 for Hall effect door state sensor, and D3 for servo control
 */ 
//  D7 = Rx = GPIO13
//  D8 =  DEBUG OUTPUT = GPIO15
#define SERVOLINE 0  // pwm out pin for door servo = D3 = GPIO0
#define SENSEPIN 5   // D1 = Hall Effect Sensor (active LOW)
#define DELAY 100    // delay in millisecs between cap tests
#define SensorRate 9600
#define INDEX_SIZE 48 // buffer size set to 48 char 
#define tag1 1402397
#define tag2 13535691
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Wifi_Credentials.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Servo.h>  // esp8266 servo lib

char rxbuffer[INDEX_SIZE] = {}; //  receive buffer
unsigned long ID=0;                   // 
int buffptr = 0; // position in circular buffer above
ESP8266WiFiMulti WiFiMulti;
Servo myservo;  // create servo object to control a servo
int pos;        // desired position of servo
int DoorState, lastDoorState;  // state of Hall effect sensor on door

void setup()                    
{
   pinMode(SENSEPIN, INPUT); 
   DoorState = HIGH;
   lastDoorState = DoorState;       // init door state change = same = NO
   Serial.begin(SensorRate);
   Serial.swap();                 // reassign serial uart 0 to GPIO15,13
   pos=0;
   doorlatch(pos);              // home servo
   WiFi.mode(WIFI_STA);
   WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());   
}

void loop()                    
{
 ArduinoOTA.handle(); 
 DoorState = digitalRead(SENSEPIN);
   if (DoorState != lastDoorState) {
    Serial.print("\nDoor state change:"); 
   // if the state has changed and change is to LOW door is closed
   if (DoorState == LOW) {
        pos=200; doorlatch(pos);             //if door clowsed, ensure latch also closed
        Serial.print("Door closed"); 
        }
     else { 
     pos=0; doorlatch(pos);             //if door open, ensure latch also open
     Serial.print("Door open"); 
      if ((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;
        http.begin("http://35.184.205.108/Debug/Showdiags.php?Catalert=Y"); //HTTP
        int httpCode = http.GET();
        if ((httpCode > 0) && (httpCode == HTTP_CODE_OK)) {
              Serial.print(": alert sent");
          }
          else {
      Serial.print("[HTTP] GET... failed, error: ");
      Serial.print(http.errorToString(httpCode).c_str());
          }
      }
        }
     lastDoorState=DoorState;        // store latest door state   
    }        
 
 if(Serial.available()>0)
   {
   buffptr=0; ID=0;
   while(Serial.available()>0)
       {
        rxbuffer[buffptr]=Serial.read();
        buffptr++;
       }              // end while serial
   for(int i=0;i<buffptr;i++)
      {     
       Serial.write(rxbuffer[i]);
       ID <<= 8;      // shift left 8 bits, add new byte to ID number
       ID |= rxbuffer[i];
      }
   Serial.write('-'); 
   Serial.print(ID,DEC);
   Serial.write('\n');  
   if(ID==tag1 || ID==tag2)  // if authorised tag detected, 
      {
      pos=0;
      doorlatch(pos);        // activate door servo
      Serial.print("tag read\n");
      }
   else { Serial.print("\nunidentified tag"); }   
   }
delay(200);     
}
void doorlatch(int pos) {
    myservo.attach(SERVOLINE);  // attaches the servo to pwm line
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(100);                       // waits 100ms for the servo to reach the position
    myservo.detach();                 // detach pwm line
  }
