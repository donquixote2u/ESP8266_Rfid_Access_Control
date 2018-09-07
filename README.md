# Esp8266_Rfid_Access_Control
 Control e.g. cat door using serial rfid reader (HZ-1050) , send wifi alert
 
 /* if RF reader detects an authorised tag, fire door control solenoid
 *  RF reader outputs ID of tag read onto serial port
 * ESP8266 uses D1,D2  for rf reader soft serial port rx and debug tx, 
 *  and D3 for digital LOW alert
 */ 
