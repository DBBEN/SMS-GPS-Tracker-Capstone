#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

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
 * 
 * @def SEND_LOCATION_INTERVALTIME 
 *  - interval time for sending location via sms in seconds
 * ----------------------------------------------------------------*/
#define SEND_LOCATION_INTERVALTIME              5  

/* ----------------------------------------------------------------
 *  DEVICE PINOUTS 
 * ----------------------------------------------------------------*/
//UART Communication Pinouts
#define NEO6M_TX                                 8
#define NEO6M_RX                                 9
//GPIO Pinouts
#define BOARD_LED                               13

unsigned long previousTime = 0;
double curr_gps_latitude, curr_gps_longitude;

SoftwareSerial NEO6M(NEO6M_TX, NEO6M_RX);
TinyGPSPlus GPS;

boolean isTime(unsigned long interval){
   unsigned long now = millis();
   if(now - previousTime > interval){
      previousTime = now;
      return 1; 
   }

   else{
      return 0;
   }
}

boolean gps_location_fixed(void){
    while(NEO6M.available() > 0){
        if(GPS.encode(NEO6M.read())){
            if(GPS.location.isValid()) return 1;
            else return 0;
        }
    }
}

void setup() {
    Serial.begin(9600);
    NEO6M.begin(9600);

    while(!gps_location_fixed());
    Serial.println("Connected!\n");
}

void loop() {
    while(NEO6M.available() > 0){
        if(GPS.encode(NEO6M.read())){
            if(GPS.location.isValid()){
                curr_gps_latitude = GPS.location.lat();
                curr_gps_longitude = GPS.location.lng();
            }   
        }
    }

    if(isTime(SEND_LOCATION_INTERVALTIME * 1000)){
        #ifdef SMS_GPS_OUTPUT_HYPERLINK
            Serial.print("http://maps.google.com/maps?q=loc:");
            Serial.print(curr_gps_latitude);
            Serial.print(',');
            Serial.println(curr_gps_longitude);
        #endif

        #ifndef SMS_GPS_OUTPUT_HYPERLINK
            Serial.print(curr_gps_latitude);
            Serial.print(',');
            Serial.println(curr_gps_longitude);
        #endif
    }
}