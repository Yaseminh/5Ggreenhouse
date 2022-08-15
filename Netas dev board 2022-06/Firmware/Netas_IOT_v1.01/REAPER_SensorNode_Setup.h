#ifndef  REAPER_SensorNode_Setup_H
#define  REAPER_SensorNode_Setup_H

#include "Arduino.h"
#include "Print.h"


//################################################################################################
//*** Definition of variables and pins and libraries **
//################################################################################################


//************************************************************************************************
//*** WiFi Network and ThingSpeak cloud user settings ***
//************************************************************************************************
#include "USER_Definitions.h"
char ssid[] = USER_SSID;        // your network SSID (name)
char pass[] = USER_password;    // your network password


//************************************************************************************************
//*** WiFi ESP8266 module ***
//************************************************************************************************
#define TS_ENABLE_SSL           //secure HTTPS connection
#include <ThingSpeak.h>         //install library for ThingSpeak
#include <Wire.h>
#include <WiFiEsp.h>            //can't recall where I got this library from 


//************************************************************************************************
//*** ThingSpeak ***
//*** https://www.mathworks.com/help/thingspeak/
//***********************************************************************************************
int status = WL_IDLE_STATUS;
WiFiEspClient  client;
// Initialize values
String myStatus = "";
char server[] = USER_SERVER; // ThingSpeak Server
//char jsonBuffer[1500] = "["; // Initialize the jsonBuffer to hold data
int sample_length = 0 ; 


//************************************************************************************************
//*** Timer Interrupt ***
//************************************************************************************************
#include <TimerInterrupt.h>
volatile boolean ISR_CHKHDT_flag = false;


//************************************************************************************************
//*** Variable for time deriven operations ***
//************************************************************************************************
unsigned long lastConnectionTime = 0; // Track the last connection time
unsigned long lastUpdateTime = 0; // Track the last update time
const unsigned long postingInterval = 5000; // Post data to ThingSpeak every 10 seconds // L to force the constant into a long data format
const unsigned long updateInterval =   2000; // Read Measurments every 5 seconds
int updateflag = 0 ; //flag that triggers empting the JSONbuffer

//************************************************************************************************
//*** DHT sensor ***
//*** DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
//*** Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
//*** See guide for details on sensor wiring and usage: https://learn.adafruit.com/dht/overview
//************************************************************************************************
//#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 2     // Digital pin connected to the DHT sensor 

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11       // DHT 11
#define DHTTYPE    DHT22      // DHT 22 (AM2302)
//#define DHTTYPE    DHT21      // DHT 21 (AM2301)
//float temp_humidity_measurments[2];
DHT_Unified dht(DHTPIN, DHTTYPE);


//************************************************************************************************
//*** Soil Moisture Sensor ***********************************************************************
//  VCC: 3
//  GND: GND
//  Analog: A0
#define SMPWRPIN 3    // Digital pin connected to provide 5V to the moisture sensor 
int smDataPin = A0;   //Analog input from sensor


//************************************************************************************************
//*** Digital Light Sensor 
//************************************************************************************************
#include <BH1750.h>  //Library from https://github.com/claws/BH1750
BH1750 lightMeter;


//************************************************************************************************
//*** PIR sensor
//************************************************************************************************
int PIRDataPin = 4;   //Analog input from sensor
int MotionDetected;   //Since it is binary data no avergae is performed  
int inst_PIR ; 

//************************************************************************************************
// ******* Real time clock  ***** 
//************************************************************************************************
//#include <TimeLib.h> // from https://github.com/PaulStoffregen/Time
// ******* RTC Needed Library ***** //
#include <Wire.h>
#include <ds3231.h> //Library from https://github.com/rodan/ds3231, also installed on the NAS
struct ts th;  //Defining a structure that stores time data
#include <RTClib.h>  // Library from https://github.com/adafruit/RTClib
DateTime now ;
RTC_DS3231 rtc;


//************************************************************************************************
//*** LCD ****
//************************************************************************************************
#include <LiquidCrystal_I2C.h>  //library for LCD via I2C
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD I2C address to 0x27


#endif
