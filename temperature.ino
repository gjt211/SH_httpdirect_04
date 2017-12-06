#ifdef _ENA_TEMP
boolean tempsensor_ok = false;
char tvalue[10]= {  '0', '.', '0', '\0' };
char old_tvalue[10]= {  '0', '.', '0', '\0' };
float tc1_array[10];
#endif

#ifdef _ENA_TEMP2
boolean tempsensor_ok2 = false;
char tvalue2[10]= {  '0', '.', '0', '\0' };
char old_tvalue2[10]= {  '0', '.', '0', '\0' };
float tc2_array[10];
#endif



//===========================================================================
#ifdef _ENA_TEMP
void readDS18B20(){

  static boolean firstAdc = true;
  static uint8_t avCnt = 0;


  //Variables for use within this function
  float dstempA;           // Raw float value from DS18B20
  float tcA = 0;           // Calculated average from 10 readings of dstemp

  printHeap();
  Serial.print(F("[MEAS] {TC1 } "));
  
  //Get the Temperature from the DS18B20
  // 85 or -127 means no data yet.
  dstempA = DS18B20A.getTempCByIndex(0);
  if (dstempA == 85.0 || dstempA == (-127.0)) {
    Serial.println(F("not found!"));
    tempsensor_ok = false;
    DS18B20A.requestTemperatures();
    yield();
  } else {
    // Read DS18B20 is Ok, so process the value

    Serial.print(dstempA,4);
    Serial.print(F(" *S+O="));
    //Apply the slope & offset
    dstempA = dstempA * configuration.tc1slope + configuration.tc1offset;
    Serial.print(dstempA,4);
    Serial.print(F(" ~ "));

    //Fill the 10 slot averaging array on first run
    if (firstAdc == true){
      firstAdc = false;
      for (uint8_t i=0;i<10;i++){
        tc1_array[i] = dstempA;
      }
    }
    
    tc1_array[avCnt] = dstempA;         //Put the current reading in the averaging array
    avCnt++;                              //Increment the averaging array position
    //If we are at the end of the averaging array, reposition at the beginning.
    if (avCnt > 9) {
      avCnt = 0;
    }
    //Add all items in the averaging array together, then divide by the number of positions
    for (int i=0;i<10;i++){
      tcA += tc1_array[i];
    }
    tcA = tcA / 10;
    yield();

    //Print the average value
    //dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
    //Put tc_float (temperature) into a CHAR array
    dtostrf(tcA, 3, 1, tvalue);
    Serial.print(tvalue);
    Serial.println(F( " C"));
  
    tempsensor_ok = true;
    DS18B20A.requestTemperatures();
    yield();
  }

  // -------------------------------------------------------
  #ifdef _ENA_TEMP2
  //Variables for use within this function
  float dstempB;
  float tcB = 0;
  static boolean firstAdc2 = true;
  static uint8_t avCnt2 = 0;
  

  printHeap();
  Serial.print(F("[MEAS] {TC2 } "));

  //Get the Temperature from the DS18B20
  // 85 or -127 means no data yet.
  //rawtc_value = DS18B20.getTemp(0);
  dstempB = DS18B20B.getTempCByIndex(0);
  if (dstempB == 85.0 || dstempB == (-127.0)) {
    Serial.println(F("not found!"));
    tempsensor_ok2 = false;
    DS18B20B.requestTemperatures();
    yield();
  } else {
    // Read DS18B20 is Ok, so process the value
    Serial.print(dstempB,4);
    Serial.print(F(" *S+O="));
    //Apply the slope & offset
    dstempB = dstempB * configuration.tc2slope + configuration.tc2offset;
    Serial.print(dstempB,4);
    Serial.print(F(" ~ "));

    //Fill the 10 slot averaging array on first run
    if (firstAdc2 == true){
      firstAdc2 = false;
      for (uint8_t i=0;i<10;i++){
        tc2_array[i] = dstempB;
      }
    }
    //Put the current reading in the averaging array
    tc2_array[avCnt2] = dstempB;    //Float
    //Increment the averaging array position
    avCnt2++;
    //If we are at the end of the averaging array, reposition at the beginning.
    if (avCnt2 > 9) {
      avCnt2 = 0;
    }
    //Add all items in the averaging array together, then divide by the number of positions
    for (int i=0;i<10;i++){
      tcB += tc2_array[i];     //Float
    }
    tcB = tcB / 10;
    yield();
  
    //Print the average value
    //dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
    //Put the float tc (temperature) into a CHAR array
    dtostrf(tcB, 3, 1, tvalue2);
    Serial.print(tvalue2);
    Serial.println(F( " C"));
  
    tempsensor_ok2 = true;
    DS18B20B.requestTemperatures();
    yield();
  }
  #endif


}
#endif
//=====================================================================================
#ifdef _ENA_TEMP
void sendTemp(){

  //Dont send data if sensor not found

  static uint8_t send_anyway_tc = 0;

  
  if (tempsensor_ok == true){
    //********** TEMPERATURE *************
    //Check if the current value is the same as the previous value
    //strcmp returns zero if true
    if((strcmp(tvalue, old_tvalue) != 0)  || (send_anyway_tc == 15)){
      send_anyway_tc = 0;
      //The old and new values don't match so copy the new value into the old value
      strncpy(old_tvalue, tvalue, 10);
      tcp_send_temp("dat",tvalue,"OUT","TC1");
    } else {
      send_anyway_tc++;
      printHeap();
      Serial.println(F("[SEND] {TC1 } not changed"));
    }
  }



  #ifdef _ENA_TEMP2
  static uint8_t send_anyway_tc2 = 0;

  if (tempsensor_ok2 == true){
    //********** TEMPERATURE 2 *************
    //Check if the current value is the same as the previous value
    //strcmp returns zero if true
    if((strcmp(tvalue2, old_tvalue2) != 0)  || (send_anyway_tc2 == 15)){
      send_anyway_tc2 = 0;
      //The old and new values don't match so copy the new value into the old value
      strncpy(old_tvalue2, tvalue2, 10);
      tcp_send_temp("dat",tvalue2,"OUT","TC2");
    } else {
      send_anyway_tc2++;
      printHeap();
      Serial.println(F("[SEND] {TC2 } not changed"));
    }
  }
  #endif


  
}
#endif
