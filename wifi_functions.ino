

void wifi(){
  Serial.println("---Wifi is ON---");
  sds011_sleep();
  sds_sleep = 1;
  if (digitalRead(displayPin) == HIGH){
      delay(3000);
      lcd.clear();
      lcd.print("Wifi is ON");
      }
  WiFiClient  client;
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  WiFi.hostname(NEWHOST);
    
    WiFiManager wifiManager;  
    wifiManager.setTimeout(30);  // время работы в режиме AP, если не введен пароль и SSID
    wifiManager.setAPCallback(configModeCallback);  // переходит на подпрограмму если не удалось подключиться к сохраненным данным SSID и pass
    wifiManager.setDebugOutput(false);  // отключает вывод отладки в serial
    wifiManager.setHostname(NEWHOST);  // hosname в режиме STA - обычный режим работы
    wifiManager.autoConnect(NEWHOST);  // hosname в режиме AP - режим ввода SSID и pass
    if (WiFi.status() == WL_CONNECTED) {
      if (digitalRead(displayPin) == HIGH){
      lcd.clear();
      lcd.print("SSID: ");
      lcd.print(WiFi.SSID());
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.print("Connected...");
      }
      } else {
        if (digitalRead(displayPin) == HIGH){
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("NOT Connected...");
          delay(500);
          }
      }
    delay_(500);
   
     ThingSpeak.setField(1, pm_pm25);
     ThingSpeak.setField(2, pm_pm10);
     ThingSpeak.setField(5, voltage);

     int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
     if(x == 200){
      Serial.println("Channel update successful.");
        if (digitalRead(displayPin) == HIGH){
          delay(1000);
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print("Updated...");
          }      
      } else {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
          if (digitalRead(displayPin) == HIGH){
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
           lcd.print("NOT Updated...");
           }
      }         
}


void configModeCallback (WiFiManager *myWiFiManager) {
    Serial.println("Entered config mode...");
  if (digitalRead(displayPin) == HIGH){
    lcd.clear();
    lcd.print("SSID: ");
    delay(500);
    lcd.print(WiFi.SSID());
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("NOT Connected...");
    delay(2000);
    lcd.clear();
    lcd.print("Entered");
    lcd.setCursor(0, 1);
    lcd.print("   config mode..");
    delay(2000);
    lcd.clear();
    lcd.print("AP: ");
    lcd.print(myWiFiManager->getConfigPortalSSID());
    lcd.setCursor(0, 1);
    lcd.print("IP: ");
    lcd.print(WiFi.softAPIP());
    }
}
