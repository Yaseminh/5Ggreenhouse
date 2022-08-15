#define FILENAMEVER  "Netas_IOT_v1.01"
/*                                                                                                                                                                                                                                                                                     /*
  Sensor Node V1

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
                                                                                                                   

  Soil Moisture Sensor on pin A0
    A)  --> A0

  RTC
    GND --> GND
    VCC --> 5V
    SCL --> pin 21
    SDA --> pin 20
   
   Digital Light Sensor
    Vin --> 5V
    GND --> GND
    SCL --> pin 21
    SDA --> pin 20
    ADDR -->GND 
  
  PIR Sensor
    Vin --> 5V
    Out --> 4
    GND --> GND

    
  Use Arduini IDE serial monitor to see the output of the program.
  Collects data  and saves in JSON buffer.
  Uses RTC to time stamp the data. 
  Sends JSON data to ThingSpeak 
  Checks if the WIFI is down and tries to reconnect.
  The Thingspeak channel assigned to this version of the code is made public. 
  To acsess the channel view use: https://thingspeak.com/channels/1765897
  The intent of this file is to be easily modifiable to talk to NetashIon instead of ThingSpeak.
  To make thr code tal to NetashIon changes only be nade to made in the  httpRequest() function.
  HTTP posting format can be done based on Netas_IOT_v0.01
  

  *** Version History ***
  Branches from TestbedV0

  v1  

  Created 12/12/2021 by N. Elbeheiry
  Edited  13/06/2022 by N. Elbeheiry
  Renewable Energy & Advanced Power Electronics Research Laboratory
*/


#include "REAPER_SensorNode_Setup.h"
#include "USER_Definitions.h"
#include "REAPER_Companion.h"



void setup() {
 Serial.begin(115200);   // initialize serial for USB debugging
  Wire.begin();
  Serial.println();
  Serial.println(F("**********************************************"));
  Serial.println(F("*** REAPERlab Smart Agriculture SensorNode ***"));
  Serial.println(F("**********************************************"));
  Serial.print(F("*** Software: "));
 
  Serial.println(F("***********************************************"));
  Serial.println();


  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++ ESP WiFi module SETUP +++
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Serial3.begin(115200);  // initialize serial for ESP module
  WiFi.init(&Serial3);    // initialize ESP module
 

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WiFi module not detected."));
    // don't continue
    while (true);
  }
  Serial.println(F("WiFi module detected."));
  Serial.print(F("Firmware v"));
  Serial.println(WiFi.firmwareVersion());

  
  // WiFi MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print(F("Connecting to WPA SSID: "));
   Serial.println(USER_SSID);

    status = WiFi.begin(USER_SSID , USER_password);      // Connect to WPA/WPA2 network
  }

  Serial.println(F("Connected to the network"));
   Serial.println(USER_SSID);


  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++ LCD display CHECK and SETUP +++
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  int error;
  Serial.print(F("Check for LCD ... "));
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print(F("Error: "));
  Serial.print(error);
  if (error == 0) {
    Serial.println(F(": LCD found."));
    lcd.begin(20, 4); // initialize the lcd: 20 characters x 4 rows
  } else {
    Serial.println(F(": LCD not found."));
  } // end if

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++ REAPERlab splash screen +++
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Serial.println(F("  Writing LCD splashscreen."));
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F(" REAPER laboratory  "));
  lcd.setCursor(0, 1);
  lcd.print(F("     SensorNode     "));


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //+++ DHT sensor SETUP +++
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

  //************************************************************************************************
  //*** Digital Light Sensor Setuo
  //************************************************************************************************
   // Initialize the I2C bus (BH1750 library doesn't do this automatically)
   Wire.begin(); // Initialize the I2C bus (BH1750 library doesn't do this automatically)
   lightMeter.begin();


  
  //************************************************************************************************
  //*** setup timer ***
  //************************************************************************************************
  //START TIMER SETUP
  //TimerInterrupt SETUP for highly preceise timed measurements
  Serial.println();
  Serial.println(F("Initialize Timer Interrupt ... "));

  Serial.println(BOARD_TYPE);
  Serial.println(TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));
  //  Serial.print(F("Timer Interval = ")); Serial.println(TIMER1_INTERVAL_MS);

  cli();                    //stop all interrupts
  noInterrupts();           //stop all interrupts
  // turn on CTC mode
  TCCR1A = 0;               // set entire TCCR1A register to 0
  TCCR1B = 0;               // same for TCCR1B
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for prescaler 8
  //TCCR1B |= (1 << CS11);    //8 prescaler
  // Set CS10 and CS12 bits for prescaler 1024
  TCCR1B |= (1 << CS10);    //1 prescaler
  TCCR1B |= (1 << CS12);    //256 prescaler
  //initialize counter value to 0;
  TCNT1  = 0;
  // set timer count for 1Hz increments
  // OCR1A = 64499;            // = (16*10^6) / (256 * 1Hz) - 1
   OCR1A = 62499;            // = (16*10^6) / (1024 * 0.25HZ) - 1
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  #define TIMER1_PERIOD 8000    //Timer 1 period in mS
  #define TIMER1_PRESCALE 1024   //prescaler
  Serial.print(F("Timer1 Period = ")); Serial.println(TIMER1_PERIOD + String(" mS"));
  OCR1A = (F_CPU / TIMER1_PRESCALE * TIMER1_PERIOD / 1000 ) - 1;
  TIMSK1 |= (1 << OCIE1A);      // enable timer compare interrupt

  delay(1000);              //delay 1000ms
  sei();                    //allow interrupts
  interrupts();             //allow interrupts 

    //************************************************************************************************
    // ******* RTC Setup  ***** //
    //************************************************************************************************
    if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
     }//end of if 
   //************************************************************************************************
      
   DS3231_get(&th);

  Serial.print( "Time reading according to the PC clock:");
  Serial.println("Date: ");
  Serial.print(th.mday);
  Serial.print("/");
  Serial.print(th.mon);
  Serial.print("/");
  Serial.print(th.year);
  Serial.print("\t Hour : ");
  Serial.print(th.hour);
  Serial.print(":");
  Serial.print(th.min);
  Serial.print(".");
  Serial.println(th.sec);
  
  Serial.print("end of setup");

} // end SETUP

void loop() {

char jsonBuffer[1250] = "["; // Initialize the jsonBuffer to hold data 
                             // JSON variable is initialised in the main to reduce the dynamic memory cnsumption
                             // The following while loop is to ensure the initialization occurs onle when needed
                             // This is to limit empting the sensor readings before posting to ThingSpeak  
updateflag = 0 ;  
int number = 0;





while( updateflag == 0 ){
 Serial.println("ISR_CHKHDT_flag");
Serial.println(ISR_CHKHDT_flag);
  
  if (ISR_CHKHDT_flag == true) {

    number = number + 1;
    Serial.println("number");
    Serial.println(number);
    float tempreture = Get_DHT_Temp();                                                                                                                                                                                                
    float humidity = Get_DHT_Humidity();
    int soil_mositure = Get_soil_mositure();
    int light_intensity = Get_light_intensity();
    int inst_PIR = Get_PIR() ; 
    char time_String[23]="\"" ;
    get_time_stamps(time_String) ; 
      if (tempreture<2){
    Serial.print(F("*** Temp<2: "));
     lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F(" REAPER laboratory  "));
  lcd.setCursor(0, 1);
  lcd.print(F("     SensorNode     <2"));
    }
    updatesJson(jsonBuffer, time_String,tempreture,humidity,soil_mositure,light_intensity,inst_PIR);

  }//end of if 
        Serial.print("millis");
        Serial.print(lastConnectionTime);
        
         Serial.print("lastConnectionTime");
        Serial.print(lastConnectionTime);
        
   if (millis() - lastConnectionTime >=  postingInterval  )  {
          size_t len = strlen(jsonBuffer);
          jsonBuffer[len-1] = ']';
           //httpRequest(jsonBuffer);
           Serial.print("jsonBuffer");
           Serial.print(jsonBuffer);
           httpRequest2(jsonBuffer);
           client.flush(); //Waits until all outgoing characters in buffer have been sent. 
           client.stop();
  }// end of if 
    client.flush(); //Waits until all outgoing characters in buffer have been sent. 
    client.stop();
 }   //end of while  
}//end of main 
