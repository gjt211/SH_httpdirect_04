// =============================================================
// Received command must be terminated with a carriage return
void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\r';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}
void showNewData() {
    if (newData == true) {
        Serial.print("$[CMD] ");
        Serial.println(receivedChars);
        newData = false;
        command_interperter(receivedChars);
    }
}
//================================================================
void command_interperter(char* payload){
  // ---------------------------------

  if (strcmp(payload, "help") == 0)
  {
    print_help();
    return;
  }
  if (strcmp(payload, "resetwifi") == 0)
  {
    reset_wifi = true;
    return;
  }
  if (strcmp(payload, "resetsensors") == 0)
  {
    configuration.tc1slope = 1.0;
    configuration.tc1offset = 0.0;
    configuration.tc2slope = 1.0;
    configuration.tc2offset = 0.0;
    configuration.CntSlope = 1.0;
    configuration.CntOffset = 0.0;
    store_config();
    return;
  }
  if (strcmp(payload, "reboot") == 0)
  {
    reboot = true;
    return;
  }
  if (strcmp(payload, "restartwifi") == 0)
  {
    WiFi.disconnect();
    yield();
    delay(100);
    WiFi.begin(configuration.my_ssid, configuration.my_psk);
    reboot = true;
    return;
  }

  
  //Micro command parser
  char v[32];
  char k[32];
  
  char *sp = strchr(payload, 61);   //61 is INT value of =
  if (sp != NULL)
  {
    strcpy(v, sp + 1);
    *sp = '\0';
    strcpy(k, payload);
    // ---------------------------------
    #ifdef _ENA_TEMP
    if (strcmp(k, "tc1slope") == 0)
    {
      configuration.tc1slope = atof(v);
      Serial.print(F("[CONF] {TC1} slope = "));
      Serial.println(configuration.tc1slope, 6);
      store_config();
      return;
    }
    // ---------------------------------
    if (strcmp(k, "tc1offset") == 0)
    {
      configuration.tc1offset = atof(v);
      Serial.print(F("[CONF] {TC1} offset = "));
      Serial.println(configuration.tc1offset, 6);
      store_config();
      return;
    }
    #endif
    // ---------------------------------
    #ifdef _ENA_TEMP2
    if (strcmp(k, "tc2slope") == 0)
    {
      configuration.tc2slope = atof(v);
      Serial.print(F("[CONF] {TC2} slope = "));
      Serial.println(configuration.tc2slope, 6);
      store_config();
      return;
    }
    if (strcmp(k, "tc2offset") == 0)
    {
      configuration.tc2offset = atof(v);
      Serial.print(F("[CONF] {TC2} offset = "));
      Serial.println(configuration.tc2offset, 6);
      store_config();
      return;
    }
    #endif
    // ---------------------------------
    #ifdef _ENA_CNTR
    if (strcmp(k, "cnt1slope") == 0)
    {
      configuration.CntSlope = atof(v);
      Serial.print(F("[CONF] {CNT} slope = "));
      Serial.println(configuration.CntSlope, 6);
      store_config();
      return;
    }
    if (strcmp(k, "cnt1offset") == 0)
    {
      configuration.CntOffset = atof(v);
      Serial.print(F("[CONF] {CNT} offset = "));
      Serial.println(configuration.CntOffset, 6);
      store_config();
      return;
    }
    if (strcmp(k, "cnt1seconds") == 0)
    {
      configuration.CntSeconds = atol(v);
      Serial.print(F("[CONF] {CNT} seconds = "));
      Serial.println(configuration.CntSeconds);
      store_config();
      ts.disable(TASK_send_CNTR);
      ts.remove(TASK_send_CNTR);
      mSinterval = configuration.CntSeconds * 1000;
      ts.add(TASK_send_CNTR, mSinterval, [&](void*) { sendCNTR(); }, nullptr, false);
      ts.enable(TASK_send_CNTR);
      return;
    }
    #endif
    // ---------------------------------
    if (strcmp(k, "apikey") == 0)
    {
      strcpy( configuration.sapikey, v );
      Serial.print(F("[CONF] {KEY} = "));
      Serial.println(configuration.sapikey);
      store_config();
      return;
    }
    // ---------------------------------
    if (strcmp(k, "host") == 0)
    {
      strcpy( configuration.shost, v );
      Serial.print(F("[CONF] {HOST} = "));
      Serial.println(configuration.shost);
      store_config();
      return;
    }
    // ---------------------------------
    if (strcmp(k, "port") == 0)
    {
      strcpy( configuration.sport, v );
      Serial.print(F("[CONF] {PORT} = "));
      Serial.println(configuration.sport);
      store_config();
      return;
    }
    // ---------------------------------
    if (strcmp(k, "ssid") == 0)
    {
      strcpy( configuration.my_ssid, v );
      Serial.print(F("[CONF] {SSID} = "));
      Serial.println(configuration.my_ssid);
      store_config();
      return;
    }
    // ---------------------------------
    if (strcmp(k, "psk") == 0)
    {
      strcpy( configuration.my_psk, v );
      Serial.print(F("[CONF] {PSK} = "));
      Serial.println(configuration.my_psk);
      store_config();
      return;
    }
    // ---------------------------------
    
  }
  Serial.println(F("[OS] Syntax error!"));

}

