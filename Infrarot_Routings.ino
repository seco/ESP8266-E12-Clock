void irdecode() {
  if (irrecv.decode(&results)) {
    if(IRDEBUG == 1){Serial.println("");}
    if(IRDEBUG == 1){Serial.println("IR Empfang");}
    if(IRDEBUG == 1){Serial.print("Code Hex : ");}
    if(IRDEBUG == 1){Serial.print(results.value, HEX);}
    if(IRDEBUG == 1){Serial.print("    Code Dec: ");}
    if(IRDEBUG == 1){Serial.println(results.value, DEC);}
  switch(results.value, DEC)
  {
  default: 
    ProgrammUnknow();
    break;
  }
    irrecv.resume(); // Receive the next value
  }
 //delay(500); 
}

void ProgrammUnknow() {
  //clientips.connect(ips, 10002);
    clientips.print(results.value, DEC); 
//    clientips.println("\r\n"); 
  //clientips.stop();
  delay(1500);
}

