void FBCheck() {
  /// Fritzbox Status überprüfen und wenn nicht neu verbinden
  if((millis() - connectioncheck) > CHECKCONNECTION and Fritzbox.status() != 4 and otaflash == 0)
  {
    if(FBDEBUG == 1){Serial.println("Fritzbox neu Verbinden") ;}
    Fritzbox.connect(FRITZBOX_HOSTNAME,FRITZBOX_PORT);
  }

  // Check connection (refresh)
  if((millis() - connectioncheck) > CHECKCONNECTION and otaflash == 0)
  {
    if(FBDEBUG == 1){
      //Serial.println(F("Checking connection..."));
    }
    connectioncheck = millis();
    // Send dummy data to "refresh" connection state
    Fritzbox.write("x");
  }
  int size;

  if((size = Fritzbox.available()) > 0)
  { 
    uint8_t* msg = (uint8_t*)malloc(size);
    size = Fritzbox.read(msg,size);
    msg[size-1] = '\0';

    if(DEBUG == 1)
    {
      Serial.print(F("->Msg: "));
      Serial.println((char*)msg);
    }
    uint8_t* copymsgforsplit = (uint8_t*)malloc(size);
    memcpy(copymsgforsplit, msg, size);

    // Analyze incoming msg
    int i = 0;
    String DatumZeit,Type,ConnectionID;
    char *pch, *ptr;
    char type[11];
    pch = strtok_r ((char*)copymsgforsplit,";", &ptr);
          
    while (pch != NULL)
    {

      if(FBDEBUG == 1)
      {
        Serial.print(F("    ->Splitted part "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.println(pch);
      }
      switch(i)
      {
      case 0:           // Date and Time
        DatumZeit = pch;
      break;
      case 1:           // TYPE
        strcpy(type, pch);
      break;
      case 2:           // ConnectionID
      break;
      case 3:           // Anrufende Nummer     
        if(strcmp(type, "RING")  == 0)
        {
          callin = 1;
          LCDwrite(2, 0, "                    ");
          LCDwrite(3, 0, "                    ");
        
          if(strstr((char*)msg, ";;")) // Unknown caller?
          {
            LCDwrite(2, 1, "Eingehender Anruf");
            LCDwrite(3, 1, "Unbekanter Anrufer");
            lastcallnumber = "Unbekanter Anrufer";
          }
          else
          {
            LCDwrite(2, 1, "Eingehender Anruf");
            LCDwrite(3, 4, pch);
            lastcallnumber = pch;
          }
          callintime = millis();
        }
      break;
      case 4:           // Eigene Nummer
        if(strcmp(type, "RING")  == 0)
        {
          lastcalltime = DatumZeit;
          lastcallinnumber = pch;
        }
      break;
      case 5:
      break;
      default: 
      break;
      }
       i++;
      pch = strtok_r (NULL, ";", &ptr); // Split next part
    }
    free(msg);
    free(copymsgforsplit);
  }
  if(millis() - callintime > callintimeout and callin == 1) 
  {
    callin = 0;
    LCDwrite(2, 0, "                    ");
    LCDwrite(3, 0, "                    ");

  }
}

