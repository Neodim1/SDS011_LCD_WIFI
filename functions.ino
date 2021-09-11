
void sds011_sleep(){
    WorkingStateResult state = sds.sleep();
   if (state.isWorking()) {
    Serial.println("Problem with sleeping the sensor.");
   } else {
      Serial.println("SDS011 is sleeping");
      }
}



void progress() {
      lcd.setCursor(15, 1);
      k++;
      if ( k == 4 ) k = 0;
      if ( k == 0 ) {
        lcd.print("/");
      } else if ( k == 1 ) {
        lcd.print("-");
      } else if ( k == 2 ) {
        lcd.write(byte(1));
      } else if ( k == 3 ) {
        lcd.print("|");
      }
      delay(250);
    }


void delay_(int delayed){
      if (digitalRead(displayPin) == HIGH) delay(delayed);
      }
