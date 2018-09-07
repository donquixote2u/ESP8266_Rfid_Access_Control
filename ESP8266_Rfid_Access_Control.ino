/* if RF reader detects an authorised tag, fire door control solenoid
 *  RF reader outputs ID of tag read onto serial port
 * ESP8266 uses D1,D2  for rf reader soft serial port rx and debug tx, 
 *  and D3 for digital LOW alert
 */ 
#define RX1 5 // *** D1 = Rx = GPIO5
#define TX1 4 // *** D2 =  DEBUG OUTPUT = GPIO4
#define ALERT 0  // digital out pin for threshold alert = D3 = GPIO0
#define DELAY 100    // delay in millisecs between cap tests
#define SensorRate 9600
#define INDEX_SIZE 48 // buffer size set to 48 char 
#define tag1 1402397
#define tag2 2
// #include <Wifi_Credentials.h>
#include <SoftwareSerial.h>
SoftwareSerial RFin(RX1,TX1);

char rxbuffer[INDEX_SIZE] = {}; //  receive buffer
unsigned long ID=0;                   // 
int buffptr = 0; // position in circular buffer above

void setup()                    
{
   RFin.begin(SensorRate);
   Serial.begin(SensorRate);
   pinMode(ALERT,OUTPUT);         // pulled low if tag detected
   digitalWrite(ALERT, HIGH);
}

void loop()                    
{
 if(RFin.available()>0)
   {
   buffptr=0; ID=0;
   while(RFin.available()>0)
       {
        rxbuffer[buffptr]=RFin.read();
        buffptr++;
       }              // end while serial
   for(int i=0;i<buffptr;i++)
      {     
       RFin.write(rxbuffer[i]);
       Serial.write(rxbuffer[i]);
       ID <<= 8;      // shift left 8 bits, add new byte to ID number
       ID |= rxbuffer[i];
      }
   RFin.write('-'); 
   RFin.print(ID,DEC);
   RFin.write('\n');  
   if(ID==tag1 || ID==tag2)  // if authoried tag detected, 
      {
      digitalWrite(ALERT,LOW);
      delay(200);
      digitalWrite(ALERT,HIGH);
      if(ID==tag1)
        { RFin.print("tag1 read\n"); }
      }
   }
delay(200);     
}
