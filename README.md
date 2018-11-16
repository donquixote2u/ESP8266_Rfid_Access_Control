# Esp8266_Rfid_Access_Control
 Control e.g. cat door using serial rfid reader (HZ-1050) , send wifi alert
 
 /* if RF reader detects an authorised tag, fire door control solenoid
 *  RF reader outputs ID of tag read onto serial port
 * ESP8266 reads serial port, activates door latch servo on detection of authorised tag
 * todo; monitor door status via hall effect sensor  (so latch not mispositioned if door already open)
 * todo; web interface to set rf reader in learn mode to store authorised tags
 * todo; web alert (to email/sms server?) for door open or invalid tag detected (impostor!)
 */ 
