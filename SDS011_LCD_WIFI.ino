


#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>      
 
#include "SdsDustSensor.h"
#include "ThingSpeak.h"
#include "defines.h"
#include <ESP8266WiFi.h>

 unsigned long myChannelNumber = SECRET_CH_ID;
 const char* myWriteAPIKey = SECRET_WRITE_APIKEY;
 //const char* new_host = NEWHOST;
 

 #include <LiquidCrystal_I2C.h>
 #include <Wire.h> 
 LiquidCrystal_I2C lcd(0x27,16,2); // Check I2C address of LCD, normally 0x27 or 0x3F
 uint8_t symbol[8] = {B00000, B10000, B01000, B00100, B00010, B00001, B00000, B00000};  // черточка прогресса
  
 
 int rxPin = 5;  //D1
 int txPin = 4;  //D2
 int powerPin = 12;   // D6 - пин управления p-мосфетом, включения дисплея и SDS011
 int wifiPin = 13;  // D7 - пин кнопки включения wifi
 int voltagePin = A0; // A0 - чтение напряжения батареи, делитель изменен! (см описание)
 int displayPin = 14; // D5 - пин включенного дисплея, без высокого уровня  на этом пине задержек при выводе на дисплей нет, уменьшаем энергопотребление
 int continuousPin = 3; // RX - пин измерений раз в секунду, без wifi и thingspeak
 
 int voltageValue = 0;
 int sleep_seconds = 300;
 byte k = 0, sds_sleep = 0;
 float pm_pm25, pm_pm10, voltage;

 SdsDustSensor sds(rxPin, txPin);

 void setup(){
  WiFi.mode(WIFI_OFF);

  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW); // Power up SDS and LCD via Mosfet
  pinMode(continuousPin, INPUT_PULLUP);
  pinMode(displayPin, INPUT);
  pinMode(wifiPin, INPUT_PULLUP);
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY, 1);  // при такой инициализации можно использовать RX как обычный GPIO

  sds.begin();
   
  lcd.begin(0,2);  // SDA=0 - D3, SCL=2 - D4      // In ESP8266-01, SDA=0, SCL=2               
  lcd.backlight();
  lcd.createChar(1, symbol);

  WorkingStateResult state = sds.sleep();
 }     
  

 void loop() {
   Serial.print("\n");
   sds.wakeup();
   if (digitalRead(displayPin) == HIGH){
      lcd.clear();     
      lcd.print(NEWHOST);
      Serial.print("---Dispay is ON---");
      } else Serial.print("---Dispay is OFF---");
  
   voltageValue = analogRead(voltagePin);
   voltage = voltageValue * (7.61 / 1023.00);  // 7.61 - подбирается по делитель
   Serial.print("\nBatt - ");
   Serial.print(voltage, 2);
   Serial.print(" V");
   if (digitalRead(displayPin) == HIGH){
      lcd.setCursor(0, 1);
      lcd.print("Batt = ");
      lcd.print(voltage, 2);
      lcd.print(" V");
      delay(1000);
      }

   Serial.println();
   Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
   Serial.println(sds.setQueryReportingMode().toString()); // ensures sensor is in 'query' reporting mode
   if (digitalRead(continuousPin) == LOW){
    delay(1000);
    lcd.clear(); 
    lcd.print("1 second");
    lcd.setCursor(0,1);
    lcd.print("   continuous...");
    Serial.println("\n1 second continuous...");
    delay(2000);
    goto next;
   }
   for ( byte n = 0; n <= 20; n++ ) {
    progress();
    Serial.print(n);
    Serial.print(","); // waiting 5 seconds before measuring
   }

next:
   PmResult pm = sds.queryPm();
   if (pm.isOk()) {
     Serial.println();
     Serial.println(pm.toString());   
     }else {
          Serial.print("Could not read values from sensor, reason: ");
          Serial.println(pm.statusToString());
          }
   
    pm_pm25 = pm.pm25;
    pm_pm10 = pm.pm10;

    if (digitalRead(displayPin) == HIGH){
      lcd.clear(); 
      lcd.print("Pm2.5 = ");
      lcd.print(pm.pm25,1);
      lcd.setCursor(0, 1);
      lcd.print("Pm10 = ");
      lcd.print(pm.pm10,1);
      }

    if (digitalRead(continuousPin) == LOW){
      progress();
      delay(150);
      goto next;
      }
    if (digitalRead(wifiPin) == LOW) wifi();
  
      if (sds_sleep == 0) sds011_sleep();
      if (digitalRead(displayPin) == HIGH){
      delay(1500);
      lcd.begin(0,2);      // In ESP8266-01, SDA=0, SCL=2               
      lcd.backlight();
      lcd.clear(); 
      lcd.print("Power OFF...");
      for ( byte n = 4; n > 1; n--) {
      lcd.setCursor(0,1);
      lcd.print(n-1);
      delay(1000);
      }
      }
      Serial.print("Device is sleeping for ");
      Serial.print(sleep_seconds);
      Serial.print(" seconds");
      Serial.println("\n");
      delay_(1000);
      digitalWrite(powerPin, HIGH); // Power cut SDS via Mosfet
      ESP.deepSleep(sleep_seconds*1000000);
 }


  
   
