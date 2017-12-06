// =============================================================
void startup (void){
  //Check for firmware update on boot before anything else happens
  check_for_fw_update(0);   //Zero of normal update check

  #ifdef _ENA_SWITCH
  //Set the SW1 input to its powered on value so as not to send a message on power up.
  reading = (digitalRead(__SWIN));
  counter = 0;
  current_state = reading;
  #endif

  #ifdef _ENA_CNTR
  //Set the SW1 input to its powered on value so as not to send a message on power up.
  reading = (digitalRead(__SWIN));
  counter = 0;
  current_state = reading;
  #endif

  #ifdef _ENA_TEMP
  DS18B20A.requestTemperatures();
  #endif
  #ifdef _ENA_TEMP2
  DS18B20B.requestTemperatures();
  #endif

  #ifdef __HWVER_NEW__
    //Enable the WiFi reset push button
    attachInterrupt(digitalPinToInterrupt(__PB), handleInterrupt, FALLING);
  #endif

  String myip;
  myip = WiFi.localIP().toString();

  ledmode=2;
  //If we get this far, then we have connected to the WiFi access point
  Serial.println();
  data = "cmd=ip&id=" + dev_key + "&t1=" + WiFi.localIP().toString();
  post_data(data);
  delay(10);
  data = "cmd=ssid&id=" + dev_key + "&t1=" + WiFi.SSID();
  post_data(data);
  delay(10);
  data = "cmd=psk&id=" + dev_key + "&t1=" + WiFi.psk();
  post_data(data);
  delay(10);
  data = "cmd=fw&id=" + dev_key + "&t1=" + fwver;
  post_data(data);

  ts.add(TASK_send_RSSI, 30000, [&](void*) { sendRSSI(); }, nullptr, false);
  ts.enable(TASK_send_RSSI);
  printHeap();
  Serial.println(F("[TASK] send RSSI : enabled"));

  #ifdef _ENA_TEMP
    ts.add(TASK_read_ds18b20, 6000, [&](void*) { readDS18B20(); }, nullptr, false);
    ts.add(TASK_send_TEMP, 60000, [&](void*) { sendTemp(); }, nullptr, false);
  #endif
  ts.enable(TASK_read_ds18b20);
  printHeap();
  Serial.println(F("[TASK] measure temperature : enabled"));
  ts.enable(TASK_send_TEMP);
  printHeap();
  Serial.println(F("[TASK] send temperature : enabled"));

  //Send the counter total every 5 minutes
  #ifdef _ENA_CNTR
  mSinterval = configuration.CntSeconds * 1000;
  ts.add(TASK_send_CNTR, mSinterval, [&](void*) { sendCNTR(); }, nullptr, false);
  ts.enable(TASK_send_CNTR);
  #endif
  
  #ifdef _ENA_SWITCH
  // Start osTimer (20 mS)
  user_init();
  #endif

  
}

// =============================================================
void setupPins(void){
  pinMode(__DATALED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(__STATUSLED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(__DATALED, _DATALEDOFF);
  #ifdef _ENA_SWITCH
    pinMode (__SWIN, INPUT_PULLUP);
  #endif
  #ifdef _ENA_CNTR
    pinMode (__SWIN, INPUT_PULLUP);
  #endif
  pinMode (__PB, INPUT_PULLUP);       // Used for WiFi-reset

}



// =============================================================
void printHeap(){
  Serial.print("<");
  Serial.print(ESP.getFreeHeap());
  Serial.print("> ");
}




// =============================================================
void printMillis (void){
  Serial.print("<");
  Serial.print(millis());
  Serial.print("> ");
}



// =============================================================
String getMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  String cMac = "";
  for (int i = 0; i < 6; ++i) {
    if (mac[i]<0x10) {cMac += "0";}
      cMac += String(mac[i],HEX);
      //if(i<5)
      //  cMac += ""; // put : or - if you want byte delimiters
  }
  cMac.toUpperCase();
  return cMac;
}
