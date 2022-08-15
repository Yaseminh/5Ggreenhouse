#define FILENAMEVER  "Netas_IOT_v0.01"
/*
  TempHumidity_LCD_INT

  Setup:
   Arduino Mega 2560

  20x4 LCD screen via I2C on pin 20,21
    SCL --> pin 21
    SCA --> pin 20

  Arduino ESP8266 Wifi Shield version 1.0 by WangTongze
    -serial3 on Mega
    Tx  --> pin 15 Rx3
    Rx  --> pin 14 Tx3

  DHT11 temperature and humidty sensor on pin 2
    VCC --> 5V or 3V
    GND --> GND
    S   --> pin 2

  Soil Moisture Sensor on pin A0
    A)  --> A0

  Use Arduini IDE serial monitor to see the output of the program.
  This code reads data from tempreture and huimidity, put it into JSON format and uses HTTP to send data to Netash. 
  The objective of this program is to test sending data to NetashION so it does not have any advanced functionalities. 
  You need to modiy the server name and path for NetaqsION

  *** Version History ***
  Branches from TempHumidity_LCD_v5, puts the DHT measurement into a timer interrupt
  instead of using a delay in the main loop

  Branches from Send_NetasCloud_TempHumidityV2
  Created by N.B.on: 2021-10-26
  Last edited by N.B: 2022-06-13
  Renewable Energy & Advanced Power Electronics Research Laboratory
*/


#include "USER.h"   //user definitions


//************************************************************************************************
//*** DHT sensor *********************************************************************************
// DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2     // Digital pin connected to the DHT sensor 
float inst_temperature; //saving tempreture and humidity as global variables
float inst_humidity;

 

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);



//************************************************************************************************
//*** LCD ****************************************************************************************
#include <LiquidCrystal_I2C.h>  //library for LCD via I2C
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD I2C address to 0x27

//************************************************************************************************
//*** sht ****************************************************************************************
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
 
Adafruit_SHT31 sht31 = Adafruit_SHT31();
byte degree[8] =
{
0b00011,
0b00011,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000,
0b00000
};

//************************************************************************************************
//*** WiFi ESP8266 module and ThingSpeak *********************************************************
#define TS_ENABLE_SSL         //secure HTTPS connection
//#include <ThingSpeak.h>       //install library for thing speak
//#include <Wire.h>
#include <WiFiEsp.h> //can't recall where I got this library from
#include <HttpClient.h> // Library from https://github.com/amcewen/HttpClient
WiFiEspClient  client;
int status = WL_IDLE_STATUS;
WiFiEspClient wifi;
char ssid[] = USER_SSIDS;     // your network SSID (name)
char pass[] = USER_password;  // your network password
int keyIndex = 0;             // your network key Index number (needed only for WEP)




//Dumb Server to test the code functionallity
//this server is temporary 
//If you want to use it you need to create a toilet at ptsv2.com and change the path below
//Your Domain name with URL path or IP address with path
const char* serverName = "ptsv2.com";
const char* path = "/t/evg4m-1655141170/post";


//Your Domain name with URL path or IP address with path
//const char* serverName = "demo.ionsandbox.netas.com.tr"; // I removed the path from this url
//const char* path = "/api/1.0/telemetry/gateway/devices/QatarTestDevice1234";
//const char* serverName = "20.52.17.56"; // I removed the path from this url
//const char* path = "/api/1.0/telemetry/gateway/devices/QatarTestDevice1234";



//char header="Z3JlZW5ob3VzZWdhdGV3YXk6MTIzNDU2Nzg";
//int HeaderLength= sizeof(header);
//const char* HttpClient(kContentLengthPrefix = HTTP_HEADER_CONTENT_LENGTH "Z3JlZW5ob3VzZWdhdGV3YXk6MTIzNDU2Nzg");


HttpClient http(client, serverName, 80);

void setup() {
   Serial.begin(115200);
   

 Serial.println();
  Serial.println(F("**********************************************"));
  Serial.println(F("*** REAPERlab Smart Agriculture SensorNode ***"));
  Serial.println(F("**********************************************"));
  Serial.print(F("*** Software version: "));
  Serial.println(FILENAMEVER);
  Serial.println(F("***********************************************"));
  Serial.println();
  lcd.begin(16,2);
lcd.createChar(1, degree);
 
while (!Serial)
delay(10); // will pause Zero, Leonardo, etc until serial console opens
 
Serial.println("SHT31 ");
if (! sht31.begin(0x44)) { 
Serial.println("Couldn't find SHT31");
while (1) delay(1);
}
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++ LCD display CHECK and SETUP +++
  int error;
  Serial.print(F("Check for LCD ... "));
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print(F("Error: "));
  Serial.print(error);
  if (error == 0) {
    Serial.println(F(": LCD found."));
    lcd.begin(20, 4); // initialize the lcd
  } else {
    Serial.println(F(": LCD not found."));
  } // end if

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++ REAPERlab splash screen +++
  Serial.println(F("  Writing LCD splashscreen."));
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println(" REAPER laboratory  ");
  lcd.setCursor(0, 1);
  lcd.println("    SensorNode     ");


  
 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++ ESP WiFi module SETUP ++
  
    Serial3.begin(115200);  // initialize serial for ESP module
    WiFi.init(&Serial3);    // initialize ESP module
      while (!Serial) {

       ; // wait for serial port to connect. Needed for native USB port only

      }
  // check for the presence of the shield
  Serial.print(F("\nCheck for WiFi module ... "));
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("  WiFi shield not present"));
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    
    Serial.println(F("  WiFi shield detected"));
    Serial.print(F("  Attempting to connect to WPA SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
   }
  Serial.println(F("  Connected to the network"));
//  printWifiStatus();


 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Wire.begin();

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++ DHT sensor SETUP +++
  Serial.print(F("\nCheck for DHTxx Sensor ... "));
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor found"));
  sensor_t sensor;

  // Print temperature sensor details.
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value);  Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value);  Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value);  Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value);  Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));

}



void loop() {

     Get_DHT();  //read sensor data

     
    //Send an HTTP POST request 
float t = sht31.readTemperature();
float h = sht31.readHumidity();
 
if (! isnan(t)) { // check if 'is not a number'
Serial.print("Temp *C = "); Serial.println(t);
lcd.print("Temp = ");
lcd.print(t);
lcd.write(1);
lcd.print("C");
} else {
Serial.println("Failed to read temperature");
lcd.print("Temperature Error");
}
 
if (! isnan(h)) { // check if 'is not a number'
Serial.print("Hum. % = "); Serial.println(h);
lcd.setCursor (0,1);
lcd.print("Hum. = ");
lcd.print(h);
lcd.print(" %");
} else {
Serial.println("Failed to read humidity");
lcd.setCursor (0,1);
lcd.print("Humidity Error");
}
Serial.println();
delay(1000);
lcd.clear();
    
    char httpRequestData[20] ; //string that holds data
    char temp[3];
    char temp1[3];
    size_t lengthT;
    //adding readings to the string
    strcat( httpRequestData,"\"{\"temp\":");
    lengthT = String(inst_temperature).length();
    String(inst_temperature).toCharArray(temp,lengthT+1);
    strcat(httpRequestData,temp);
    strcat(httpRequestData,",\"hum\":");
    lengthT = String(inst_humidity).length();
    String(inst_humidity).toCharArray(temp1,lengthT+1);
    strcat(httpRequestData,temp1);
    strcat(httpRequestData,"}\"");
    String data_length = String(strlen(httpRequestData)); //Compute the data buffer length
    //Start sending
    http.beginRequest();
    Serial.println(httpRequestData);
    http.post(path);
    http.sendHeader("Content-Type", "application/json");
    http.sendHeader("Content-Length", data_length);
    http.sendHeader("Authorization", "Z3JlZW5ob3VzZWdhdGV3YXk6MTIzNDU2Nzg=");
    http.beginBody();
    http.print(httpRequestData);
    http.endRequest();
    int statusCode = http.responseStatusCode();
    String response = http.responseBody();
    Serial.print("Status code: ");
    Serial.println(statusCode);
    client.stop();
    client.flush();

  delay(2000);    
}



//################################################################################################
//*** Get DHT Readings ***
//################################################################################################
void Get_DHT()
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
    Serial.print(F("Temperature:  ")); Serial.print(event.temperature, 1); Serial.println(F("°C"));
    lcd.setCursor(0, 1);
    lcd.print(F("Temperature    ")); lcd.print(event.temperature, 1); lcd.print("C");
    inst_temperature = event.temperature;
  }

  // Get humidity event and print value
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
    lcd.setCursor(0, 2);
    lcd.print(F("Humidity       error"));
  }
  else {
    Serial.print(F("Humidity:      ")); Serial.print(event.relative_humidity, 1); Serial.println(F("%"));
    lcd.setCursor(0, 2);
    lcd.print(F("Humidity         ")); lcd.print(event.relative_humidity, 0); lcd.print("%");
    inst_humidity = event.relative_humidity;
  }

}//end of get DHT
