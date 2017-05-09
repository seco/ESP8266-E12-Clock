void LCDRefresh() {
  LCDPrintTime();
  LCDPrintTemp();
}


void LCDwrite(int zeile, int spalte, String Text) {
  lcd.setCursor(spalte,zeile);
  lcd.print(Text);
}

void LCDPrintTemp() {
  if(second(local) >= 0 and second(local) <= 4 and LCDTempInnen == 1 and callin == 0 or second(local) >= 10 and second(local) <= 14 and LCDTempInnen == 1 and callin == 0 or second(local) >= 20 and second(local) <= 24 and LCDTempInnen == 1 and callin == 0 or second(local) >= 30 and second(local) <= 34 and LCDTempInnen == 1 and callin == 0 or second(local) >= 40 and second(local) <= 44 and LCDTempInnen == 1 and callin == 0 or second(local) >= 50 and second(local) <= 54 and LCDTempInnen == 1 and callin == 0)
  {
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("WZ ");
    lcd.print(ipsTempWZ);
    lcd.setCursor(10,2);
    lcd.print("SZ ");
    lcd.print(ipsTempSZ);
    lcd.setCursor(0,3);
    lcd.print("CZ ");
    lcd.print(ipsTempCZ);
    lcd.setCursor(10,3);
    lcd.print("KZ ");
    lcd.print(ipsTempKZ);
    LCDTempInnen = 0;
    LCDTempAussen = 1;
  }

  if(second(local) >= 5 and second(local) <= 9 and LCDTempAussen == 1 and callin == 0 or second(local) >= 15 and second(local) <= 19 and LCDTempAussen == 1 and callin == 0 or second(local) >= 25 and second(local) <= 29 and LCDTempAussen == 1 and callin == 0 or second(local) >= 35 and second(local) <= 39 and LCDTempAussen == 1 and callin == 0 or second(local) >= 45 and second(local) <= 49 and LCDTempAussen == 1 and callin == 0 or second(local) >= 55 and second(local) <= 59 and LCDTempAussen == 1 and callin == 0)
  {
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("Aussen ");
    lcd.print(ipsTempA);
    LCDTempInnen = 1;
    LCDTempAussen = 0;
  }
}

void printBigDigits(byte digits, int x){
  // utility function for digital clock display: prints preceding colon and leading 0
  bignum(digits/10, x);
  bignum(digits%10, x+3);
}

void LCDPrintTime(){
  lcd.setCursor(6,0);
  lcd.print("-");
  lcd.setCursor(6,1);
  lcd.print("-");
  printBigDigits(hour(local), 0);
  printBigDigits(minute(local), 7);
  LCDWochenTag(weekday(local));
  LCDDatum(day(local), month(local), year(local));
}

void LCDDatum(int Tag, int Monat, int Jahr) {
  lcd.setCursor(15,1);
  if(Tag < 10)
    lcd.print("0");
  lcd.print(Tag,DEC);
  lcd.print(".");
  if(Monat < 10)
    lcd.print("0");
  lcd.print(Monat,DEC);
  lcd.setCursor(16,0);
  lcd.print(Jahr,DEC);
}

void LCDWochenTag(int digit) {
  switch (digit)               // depending on which button was pushed, we perform an action  
  { 
  case 1:lcd.setCursor(13,0);lcd.print("So");break;
  case 2:lcd.setCursor(13,0);lcd.print("Mo");break;
  case 3:lcd.setCursor(13,0);lcd.print("Di");break;
  case 4:lcd.setCursor(13,0);lcd.print("Mi");break;
  case 5:lcd.setCursor(13,0);lcd.print("Do");break;
  case 6:lcd.setCursor(13,0);lcd.print("Fr");break;
  case 7:lcd.setCursor(13,0);lcd.print("Sa");break;
  }
}

void bignum(int digit, int x) {
  switch (digit)               // depending on which button was pushed, we perform an action  
  { 
  case 0:{custom0(x);break;}
  case 1:{custom1(x);break;}
  case 2:{custom2(x);break;}
  case 3:{custom3(x);break;}
  case 4:{custom4(x);break;}
  case 5:{custom5(x);break;}
  case 6:{custom6(x);break;}
  case 7:{custom7(x);break;}
  case 8:{custom8(x);break;}
  case 9:{custom9(x);break;}
  }
}

void custom1(int x){lcd.setCursor(x,0);lcd.write(0);lcd.write(255);lcd.write(32);lcd.setCursor(x, 1);lcd.write(1);lcd.write(255);lcd.write(2);}
void custom2(int x){lcd.setCursor(x,0);lcd.write(0);lcd.write(3);lcd.write(6);lcd.setCursor(x, 1);lcd.write(255);lcd.write(1);lcd.write(2);}
void custom3(int x){lcd.setCursor(x,0);lcd.write(3);lcd.write(3);lcd.write(6);lcd.setCursor(x, 1);lcd.write(1);lcd.write(5);lcd.write(6);}
void custom4(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(1);lcd.write(2);lcd.setCursor(x, 1);lcd.write(32);lcd.write(255);lcd.write(32);}
void custom5(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(3);lcd.write(7);lcd.setCursor(x, 1);lcd.write(1);lcd.write(5);lcd.write(6);}
void custom6(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(3);lcd.write(7);lcd.setCursor(x, 1);lcd.write(255);lcd.write(5);lcd.write(6);}
void custom7(int x){lcd.setCursor(x,0);lcd.write(0);lcd.write(0);lcd.write(6);lcd.setCursor(x, 1);lcd.write(32);lcd.write(255);lcd.write(32);}
void custom8(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(3);lcd.write(6);lcd.setCursor(x, 1);lcd.write(255);lcd.write(5);lcd.write(6);}
void custom9(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(3);lcd.write(6);lcd.setCursor(x, 1);lcd.write(1);lcd.write(5);lcd.write(6);}
void custom0(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(0);lcd.write(6);lcd.setCursor(x, 1);lcd.write(255);lcd.write(5);lcd.write(6);}

void lcdon() {
  lcd.backlight();
}

void lcddim() {
  analogWrite(LCDBackgroundLED, LCDBackgroundLEDDim);
}

void lcdoff() {
  lcd.noBacklight();
}

void lcdstartdim() {

}

