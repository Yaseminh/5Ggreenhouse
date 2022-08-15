#ifndef  REAPER_Companion_H
#define  REAPER_Companion_H


#include "Arduino.h"
#include "Print.h"
#include "REAPER_SensorNode_Setup.h"
float Tempret; 
float rssi ;
int soilm;
int li;
int PIR_temp;
//#include <WiFiClientSecure.h> 

//REAPER_Companion file contains all user defined functions used by the code SensorNodeV1
//The functions are lissted bellow
// 1 - updatesJson
  // Function Input: single reading for esch sensors and string jsonBuffer 
  // Function Output: N/A
  // Function Description: The function concatenates jsonBuffer with the sensor reading 
  // and puts it in JSON format
void updatesJson(char* jsonBuffer,char* time_String,float tempreture,float humidity,int soil_mositure, int ight_intensity, int PIR );
// 2 -  httpRequest
  // Function Input: jsonBuffer
  // Function Output: N/A
  // Function Description: The function post data to thingspeak, checks if Wifi is down and attempt to reconect
void httpRequest(char* jsonBuffer);  
// 3-Get_Sensor_name()
  // Function Input: N/A
  // Function Output: float or int with sensor readings
  // available sensors:
   // float tempreture = Get_DHT_Temp();                                                                                                                                                                                                
   // float humidity = Get_DHT_Humidity();
   // int soil_mositure = Get_soil_mositure();
   // int light_intensity = Get_light_intensity();
   // int inst_PIR = Get_PIR() ; 
// 4- get_time_stamps(time_String) ; 
  // Function Input: char that holds time string
  // Function Output: N/A
  // available sensors: reads time from RTC and sync time with the software clock based on user defined intervals
// 5- data_optimization()
  // Not set yet 
  // This function is to ...


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++ Updates the jsonBuffer with data++++++++++++++++++++++++++++++++  
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Updates the jsonBuffer with data
 void updatesJson(char* jsonBuffer,char* time_String,float tempreture,float humidity,int soil_mositure, int ight_intensity , int PIR ){
  //making sure the jsonBuffer dies not go beyond the maximum allowed
  size_t sizeof_jsonBuffer =  strrchr(jsonBuffer,'{') - 4412 + 144 ;
  Serial.println("sizeof_jsonBuffer");
  Serial.println(sizeof_jsonBuffer);
  Serial.println("sample_length");
  Serial.println(sample_length);
  if(sizeof_jsonBuffer+sample_length> 2500)
  ISR_CHKHDT_flag = true;
 
  size_t lengthT;
  char temp[16];
  Serial.println("geldim1");

  Serial.println(time_String);

  
   
  Tempret = tempreture; 
  lengthT = String(Tempret).length();
  String(Tempret).toCharArray(temp,lengthT+1);

  rssi = humidity; 
  lengthT = String(rssi).length();
  String(rssi).toCharArray(temp,lengthT+1);

  soilm = soil_mositure; 
  lengthT = String(soilm).length();
  String(soilm).toCharArray(temp,lengthT+1);


  li = ight_intensity;

  lengthT = String(li).length();
  String(li).toCharArray(temp,lengthT+1);

  PIR_temp = PIR;

  lengthT = String(PIR_temp).length();
  String(PIR_temp).toCharArray(temp,lengthT+1);

  Serial.println("geldim2");

 ISR_CHKHDT_flag = false;
}//end of UpdateJson



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++ Define the httpRequest method to send data to ThingSpeak and to print the response code from the server.++++++++++++++++++++++++++++++++  
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void httpRequest2(char* jsonBuffer){
  Serial.println("hht2 comes");
 char server[] = "greenhouse.ionsandbox.netas.com.tr";
    //Serial.println(jsonBuffer);
    char writeAPI_Key[] = USER_APIwritekey;
      /*char data[1500] = "{\"write_api_key\":\"" ;  
  strcat(data, writeAPI_Key);
  strcat(data,"\",\"updates\":"); // Replace YOUR-CHANNEL-WRITEAPIKEY with your ThingSpeak channel write API key
  strcat(data,jsonBuffer);
  strcat(data,"}");*/
   float new_soil_moisture =((float)(soilm) * 100) / (1023);
   //
   String data = "{\"Temperature\":";
   data  +=  Tempret;
  //%°C
  data  += ",\"Humidity\":";
  data  +=  rssi;
  // %
  data  += ",\"Soil_Moisture\":";
  data  +=   new_soil_moisture;
  //lx luxury
  data  += ",\"Light_Intensity\":";
  data  +=    li;
  data  += ",\"PIR_Sensor\":";
  //no unit
  data  +=     PIR_temp;
  data += "}";
  client.stop();
  client.flush();

 char* data1 = (char*)data.c_str();   
  String data_length = String(strlen(data1)+1); //Compute the data buffer length
  Serial.println(data);
  // if you get a connection, report back via serial
  Serial.println("hht2 443");
  Serial.println("status");
  Serial.println(status);
  if (client.connectSSL(server, 443)) {
     // Make a HTTP request
       Serial.println("data requests");
    client.println("POST /api/1.0/telemetry/device HTTP/1.1");
    client.println("Host: greenhouse.ionsandbox.netas.com.tr");
    client.println("Authorization: Uml6YV9CYWhjZV9EZXZpY2U6MTIzNDU2Nzg=");
    client.println("Content-Type: application/json");
    client.println("Connection:close");
    client.println("Content-Length: "+data_length);
    client.println();
    client.println(data);
    delay(300000); //Wait to receive the response
  }
    else{
  Serial.println("Failure: Failed to connect to Ion");
  delay(1000); //Wait to receive the response
    }
  Serial.println("NODEMCUWİFİ Address");
  Serial.println(WiFi.localIP());
  
  client.parseFloat();
  String resp = String(client.parseInt());
  Serial.println("Response code:"+resp); // Print the response code. 202 indicates that the server has accepted the response
    

 if( resp == "202") { //if sucessfully connected to the cloud
  updateflag = 1; //change the update flag to empty  JSONBuffer
  lastConnectionTime = millis(); //Update the last conenction time
    Serial.println("connected cloud:");
 } else { //if data are not posted, do not empty the JSONBuffer
      size_t len = strlen(jsonBuffer);
      jsonBuffer[len-1] = ','; 
        Serial.println("not connected cloud:");
     // lastConnectionTime = millis(); //Update the last conenction time
        if(status != WL_CONNECTED){
           WiFi.begin(USER_SSID , USER_password);//attemt to resore it
           Serial.println("baglantı koptu:");
             while (WiFi.status() != WL_CONNECTED) {
           delay(500);
           Serial.print(".");
          } //if Wifi is down
        }
         
      }//end of else

  }
  
/*void httpRequest(char* jsonBuffer) {
 
  char channel_id[] = USER_channel;
  char writeAPI_Key[] = USER_APIwritekey;

  char data[1500] = "{\"write_api_key\":\"" ; 
  strcat(data, writeAPI_Key);
  strcat(data,"\",\"updates\":"); // Replace YOUR-CHANNEL-WRITEAPIKEY with your ThingSpeak channel write API key
  strcat(data,jsonBuffer);
  strcat(data,"}");
  // Close any connection before sending a new request
  client.stop();
  client.flush();
  String data_length = String(strlen(data)+1); //Compute the data buffer length
  Serial.println(data);
  // POST data to ThingSpeak
  if (client.connect(server, 80)) {
  	char post_temp[] = "POST /channels/";
  	strcat(post_temp,channel_id);
  	strcat(post_temp,"/bulk_update.json HTTP/1.1");
    client.println(post_temp); // Replace YOUR-CHANNEL-ID with your ThingSpeak channel ID 
    //client.println("POST /channels/1572721/bulk_update.json HTTP/1.1"); // Replace YOUR-CHANNEL-ID with your ThingSpeak channel ID
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: mw.doc.bulk-update (Arduino ESP8266)");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("Content-Length: "+data_length);
    client.println();
    client.println(data);
  }
  else {
    Serial.println("Failure: Failed to connect to ThingSpeak");
  }
  delay(250); //Wait to receive the response
  client.parseFloat();
  String resp = String(client.parseInt());
  Serial.println("Response code:"+resp); // Print the response code. 202 indicates that the server has accepted the response

 if( resp == "202") { //if sucessfully connected to the cloud
  updateflag = 1; //change the update flag to empty  JSONBuffer
  lastConnectionTime = millis(); //Update the last conenction time
 } else { //if data are not posted, do not empty the JSONBuffer
      size_t len = strlen(jsonBuffer);
      jsonBuffer[len-1] = ','; 
     // lastConnectionTime = millis(); //Update the last conenction time
        if(status != WL_CONNECTED) //if Wifi is down
          WiFi.begin(USER_SSID , USER_password);//attemt to resore it
      }//end of else
}// end of httpRequest */ 


// ****************************************************************
// *********************Get Sensor readings************************
// ****************************************************************

float Get_DHT_Humidity( ){
  // Get humidity event and print value
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
   lcd.setCursor(0, 2);
    lcd.print(F("Humidity       error"));
  }
  else {
    Serial.print(F("Humidity:      ")); Serial.print(event.relative_humidity); Serial.println(F("%"));
     return(event.relative_humidity);
    lcd.setCursor(0, 2);
   lcd.print(F("Humidity         ")); lcd.print(event.relative_humidity, 0); lcd.print("%");
   // ThingSpeak.setField(2, event.relative_humidity);  //write to ThingSpeak channel
  }

} //end of Get_DHT_Humidity 


float Get_DHT_Temp( )
{
  Serial.println(F("================================="));
  Serial.println(F("REAPERlab Sensor Node"));
  Serial.println(F("---------------------"));

  // Get temperature event and print value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    lcd.setCursor(0, 1);
    lcd.print(F("Temperature    error"));
  }
  else {
    Serial.print(F("Temperature:  ")); Serial.print(event.temperature); Serial.println(F("°C"));
     return(event.temperature);
    lcd.setCursor(0, 1);
    lcd.print(F("Temperature    ")); lcd.print(event.temperature, 1); lcd.print("C");
   // ThingSpeak.setField(1, event.temperature);        //write to ThingSpeak channel
  }

}

int Get_soil_mositure(){
// get soil moisture sensor value from ADC
  int soilmoisture = analogRead(smDataPin);
  Serial.print(F("Soil Moisture :")); Serial.println(soilmoisture);
  return(soilmoisture);
 /*& //Output to LCD screen
  lcd.setCursor(0, 0);
  lcd.leftToRight();
  lcd.print(F("REAPERlab SensorNode"));  //20 characters
  lcd.setCursor(0, 3);
  lcd.print(F("Soil Moisture  ")); lcd.print((int)soilmoisture, 4);*/

}//end of Get_soil_mositure



int Get_light_intensity(){
 //Digital Light Sensor Readings 
 float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  return(lux);
  }//end of Get_light_intensity()


int Get_PIR()  {
  // get PIR sensor value from digital
  int inst_PIR  = digitalRead(PIRDataPin);
  Serial.print(F("Motion Detected: ")); Serial.println(inst_PIR,6);
  return(inst_PIR);
  } //end of Get_PIR


// ****************************************************************
// *********************GEt time stamps****************************
// ****************************************************************

void get_time_stamps( char* time_String ) {

    DS3231_get(&th);
    char time_temp[10];
    String(th.year).toCharArray(time_temp, 5);
    strcat(time_String, time_temp);
    strcat(time_String, "-");
    String(th.mon ).toCharArray(time_temp, 3);
    strcat(time_String, time_temp);
    strcat(time_String, "-");
    String(th.mday).toCharArray(time_temp, 3);
    strcat(time_String, time_temp);
    strcat(time_String, " ");

    String(th.hour).toCharArray(time_temp, 3);
    strcat(time_String, time_temp);
    strcat(time_String, ":");
    String(th.min).toCharArray(time_temp, 3);
    strcat(time_String, time_temp);
    strcat(time_String, ":");
    String(th.sec).toCharArray(time_temp, 3);
    strcat(time_String, time_temp);
    strcat(time_String, " -0000"); // milliseconds are not considered for now
    strcat(time_String, "\"");
   
  Serial.print("time string: ");
  Serial.println(time_String);
  ISR_CHKHDT_flag = false;
} //end of get_time_stamps function


//################################################################################################
//*** Interrupt Routines ***
//################################################################################################

ISR(TIMER1_COMPA_vect) {
 ISR_CHKHDT_flag = true;
}

void data_optimization(){   //need to call a function that try to connect to the internet and does something to the data (avergae or something ) 

if(status != WL_CONNECTED) //if Wifi is down
{
    WiFi.begin(USER_SSID , USER_password);//attemt to resore it
           while (WiFi.status() != WL_CONNECTED) {
           delay(500);
           Serial.print(".");
  }
}
                                                       
ISR_CHKHDT_flag = false;
}//end of data_optimization()

#endif
