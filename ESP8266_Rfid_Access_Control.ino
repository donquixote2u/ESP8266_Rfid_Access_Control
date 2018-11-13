/* if RF reader detects an authorised tag, fire door control solenoid
 *  RF reader outputs ID of tag read onto serial port
 * ESP8266 uses D7,D8  for rf reader alt serial port rx and debug tx, 
 *  and D3 for digital LOW alert
 */ 
// *** D7 = Rx = GPIO13
// *** D8 =  DEBUG OUTPUT = GPIO15
#define ALERT 0  // digital out pin for threshold alert = D3 = GPIO0
#define DELAY 100    // delay in millisecs between cap tests
#define SensorRate 9600
#define INDEX_SIZE 48 // buffer size set to 48 char 
#define tag1 1402397
#define tag2 13535691
// #include <Wifi_Credentials.h>

char rxbuffer[INDEX_SIZE] = {}; //  receive buffer
unsigned long ID=0;                   // 
int buffptr = 0; // position in circular buffer above

void setup()                    
{
   Serial.begin(SensorRate);
   Serial.swap();                 // reassign serial uart 0 to GPIO15,13
   pinMode(ALERT,OUTPUT);         // pulled low if tag detected
   digitalWrite(ALERT, HIGH);
}

void loop()                    
{
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
   if(ID==tag1 || ID==tag2)  // if authoried tag detected, 
      {
      digitalWrite(ALERT,LOW);
      delay(200);
      digitalWrite(ALERT,HIGH);
      Serial.print("tag read\n"); 
      }
   else { Serial.print("\nunidentified tag"); }   
   }
delay(200);     
}
