#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

/* ----------------------------------------------------------------
 *  @def RECEIVER_NUMBER 
 *  - mobile number of the SMS receiver
 *  
 * @def DEVICE_NUMBER
 *  - mobile number of the fire alarm device
 * ---------------------------------------------------------------- */
#define RECEIVER_NUMBER                         "+639085401095"
#define DEVICE_NUMBER                           "+639684408282"

/* ----------------------------------------------------------------
 *  GPS OUTPUT OPTION
 * ----------------------------------------------------------------
 * - Uncomment option 
 *
 * @def SMS_OUTPUT_RAW_GPS 
 *  - GPS output data will be raw longitude and latitude coordinates
 * 
 * @def SMS_OUTPUT_HYPERLINK 
 *  - GPS output data will be a hyperlink
 * 
 * ----------------------------------------------------------------*/
//#define SMS_GPS_OUTPUT_RAW_GPS                      
#define SMS_GPS_OUTPUT_HYPERLINK                    

/* ----------------------------------------------------------------
 *  @def CONNECTING_WAITTIME 
 *  - wait time for connecting to cellular network in seconds
 * ----------------------------------------------------------------*/
#define CONNECTING_WAITTIME                     20  

/* ----------------------------------------------------------------
 *  DEVICE PINOUTS 
 * ----------------------------------------------------------------*/
//UART Communication Pinouts
#define SIM800L_TX                              10
#define SIM800L_RX                              11
#define NEO6M_TX                                 8
#define NEO6M_RX                                 9
//GPIO Pinouts
#define BOARD_LED                               13

/* ----------------------------------------------------------------
 *  CODE MESSAGES OPTIONS
 * ----------------------------------------------------------------
 * 
 * @def SMS_DEVICE_READY 
 *  - SMS message option to notify receiver that device is ready
 * 
 * @def SMS_SEND_COORDINATES
 *  - SMS message option to transmit coordinates to receiver
 *  - URL Format "http://maps.google.com/maps?q=loc:7.1007062,125.5822249"
 * 
 * @def SMS_NO_LOCATION
 *  - SMS message option to notify receiver coordinates can't be determined
 * 
 * @ex. sim800l_sendSMS(SMS_DEVICE_READY)
 * 
 * ----------------------------------------------------------------*/
#define SMS_DEVICE_READY                         1
#define SMS_SEND_COORDINATES                     2 
#define SMS_NO_LOCATION                          3

long previousTime = 0;

SoftwareSerial SIM800L(SIM800L_TX, SIM800L_RX);
SoftwareSerial NEO6M(NEO6M_TX, NEO6M_RX);
TinyGPSPlus GPS;

void setup() {
  Serial.begin(9600);

  SIM800L.begin(9600);
  NEO6M.begin(9600);

  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, HIGH);

  //Wait for certain seconds to give time for connection
  while(!isTime(CONNECTING_WAITTIME * 1000));
  digitalWrite(BOARD_LED, LOW);
}

void loop() {
 
}


boolean isTime(long interval){
   unsigned long now = millis();
   if(now - previousTime > interval){
      previousTime = now;
      return 1; 
   }

   else{
      return 0;
   }
}

void sim800l_sendSMS(uint8_t i){
  SIM800L.println("AT+CMGF=1");
  SIM800L.print("AT+CMGS=\"");
  SIM800L.print(RECEIVER_NUMBER);
  SIM800L.println("\"");

  if(i == SMS_DEVICE_READY){
    SIM800L.println("Device is connected to network and ready for operation.");
  }
  
  else if(i == SMS_SEND_COORDINATES){
    #ifdef SMS_GPS_OUTPUT_HYPERLINK
      SIM800L.println("http://maps.google.com/maps?q=loc:");
    #endif

    #ifndef SMS_GPS_OUTPUT_HYPERLINK
      //Send raw GPS data
    #endif
  }
 
  else{
    SIM800L.println("Cannot determine location");
  }

  SIM800L.print("\n - SMS GPS Tracker");
  SIM800L.write(26);
  SIM800L.println();
  delay(1000);
}
