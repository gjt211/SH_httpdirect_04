// NEW VERSION NOT USING MQTT - HTTP POST DATA ONLY, UNABLE TO CONTROL
// Released to several customers


extern "C" {
#include "user_interface.h"
}

os_timer_t myTimer;

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <TickerScheduler.h>
#include "hwconfig.h"


#define __USE_PDO


#if defined(_ENA_CNTR) && defined(_ENA_SWITCH)
  #error Unable to use both Switch & Counter at same time
#endif
#if defined(__HWVER_NEW__) && defined(__HWVER_OLD__)
  #error Unable to compile for two hardware variants at same time
#endif


// =============================================================
#define TASK_send_TEMP          0
#define TASK_send_CNTR          1
#define TASK_read_ds18b20       2
#define TASK_send_RSSI          3
//#define TASK_check_FWupd        4
//#define TASK_sysinfo            5
// =============================================================
const byte interruptPin = __PB;
volatile byte interruptCounter = 0;
int numberOfInterrupts = 0;

struct config_t {
  char fingerprint[64] = "";
  char shost[64] = "";
  char sport[6] = "";
  float tc1slope = 1.0;
  float tc1offset = 0.0;
  float tc2slope = 1.0;
  float tc2offset = 0.0;
  float CntSlope = 1.0;
  float CntOffset = 0.0;
  char sapikey[32] = "";
  char my_ssid[32] = "";
  char my_psk[32] = "";
  unsigned long CntSeconds = 300;    // 5 minutes
} configuration;






// server.sensorhost.com SHA1 default fingerprint
char fingerprint[] = "22 36 E8 2C F3 9C E9 1B 4F D3 BC FC 89 25 58 19 D2 5A C7 51";
char shost[] = "sensorhost.com";
char sport[] = "80";
char sapikey[] = "SHGT21XLB22";

boolean reset_wifi = false;


Ticker StatusTicker;
Ticker ticker;

//Used for scheduling actions such as read temperature sensor
// Allow up to 5 ticker scheduler items
TickerScheduler ts(6);

//uint8_t status_tick_count = 0;
//uint8_t status_tick_max = 17;


String MyMac;
uint8_t ledmode = 5;      // Number of times to flash the STATUS LED

//String shp;
String dev_key;           // Stores the complete deviceID (MAX & Serial)
String command;

//unsigned long currentMillis;
//unsigned long previousRssiMillis = 0;
//const long intervalRssi = 30000;

const byte numChars = 48;       // Max qty of bytes we can receive
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;
boolean reboot = false;
String data;
uint8_t connect_counter = 0;


uint16_t hport;     // Integer version of configuration.sport

//###########################################################################
// WiFi Manager variables
//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println(F("Should save config"));
  shouldSaveConfig = true;
}



// =============================================================
void check_for_fw_update(uint8_t force);



// =============================================================
// =============================================================
void setup() {
  setupPins();

  ledmode = 4;
  StatusTicker.attach(0.2, StatusTick);    //Start the status LED flasher timer @ 200mS

  Serial.begin(115200);
  Serial.print(F("\n\n***\r\n\r\n"));
  delay(100);
  Serial.println(ESP.getResetReason());
  Serial.print(F("\n\n***\n\n"));
  delay(900);

  // Get the ESP chipID
  unsigned long thischipID = ESP.getChipId();
  //Get the WiFi MAC address
  MyMac = getMacAddress();

  dev_key = MyMac + String(thischipID);

  Serial.println(F("Sensorhost TCP/HTTP"));
  Serial.println("HW Version: " + hwver);
  Serial.println("FW Version: " + fwver);
  Serial.println("UPD Server: " + update_server);
  Serial.println("POST Server: " + post_file);
  Serial.println();

  Serial.println(F("----------------"));
  WiFi.printDiag(Serial);
  WiFi.setOutputPower(20.5);
  Serial.println(F("----------------"));

  Serial.print(F("Read EEPROM..."));
  EEPROM.begin(512);    //Allocate 512 bytes for EEPROM storage (Currently using 2 + 134 + 1 bytes)
  //Check if the configuration has been stored in EEPROM
  byte e1 = EEPROM.read(0);
  byte e2 = EEPROM.read(1);
  if ((e1 != 214) && (e2 != 26)){
    Serial.println(F("Not configured!"));
    EEPROM.write(0,214);
    EEPROM.write(1,26);
    EEPROM.commit();

    //Put the hard coded defaults into the struct
    // strncpy (destination, source, size)
    strncpy ( configuration.fingerprint, fingerprint, sizeof(configuration.fingerprint) );
    strncpy ( configuration.shost, shost, sizeof(configuration.shost) );
    strncpy ( configuration.sport, sport, sizeof(configuration.sport) );
    strncpy ( configuration.sapikey, sapikey, sizeof(configuration.sapikey) );
    /* //COMMENTED OUT SO WE DONT OVERWRITE-CLEAR SLOPE & OFFSET IF WE CHANGE THE EEPROM SETTINGS 
    configuration.tc1slope = 1.0;
    configuration.tc1offset = 0.0;
    configuration.tc2slope = 1.0;
    configuration.tc2offset = 0.0;
    configuration.CntSlope = 1.0;
    configuration.CntOffset = 0.0;
    */
    
    Serial.print(F("Write defaults.."));
    // Write the defaults from the struct to eeprom
    store_config();
    } else {
    EEPROM.get(2,configuration);
    Serial.println(F("Done."));
  }
  Serial.println(F("----------------"));
  Serial.println(F("Configuration: "));
  Serial.print("FP: ");
  Serial.println(String(configuration.fingerprint));
  Serial.print(F("Host: "));
  Serial.println(configuration.shost);
  Serial.print(F("Port: "));
  Serial.println(configuration.sport);
  Serial.print(F("API Key: "));
  Serial.println(configuration.sapikey);
  #ifdef _ENA_TEMP
  if(isnan(configuration.tc1slope)){
    configuration.tc1slope = 1.0;
    shouldSaveConfig = true;
  }
  if(configuration.tc1slope == 0.0){
    configuration.tc1slope = 1.0;
    shouldSaveConfig = true;
  }
  if(isnan(configuration.tc1offset)){
    configuration.tc1offset = 0.0;
    shouldSaveConfig = true;
  }
  Serial.print("TC1 Slope: ");
  Serial.println(configuration.tc1slope, 6);
  Serial.print("TC1 Offset: ");
  Serial.println(configuration.tc1offset, 6);
  #endif
  #ifdef _ENA_TEMP2
  if(isnan(configuration.tc2slope)){
    configuration.tc2slope = 0.0;
    shouldSaveConfig = true;
  }
  if(isnan(configuration.tc2offset)){
    configuration.tc2offset = 0.0;
    shouldSaveConfig = true;
  }
  Serial.print("TC2 Slope: ");
  Serial.println(configuration.tc2slope, 6);
  Serial.print("TC2 Offset: ");
  Serial.println(configuration.tc2offset, 6);
  #endif
  #ifdef _ENA_CNTR
  if(isnan(configuration.CntSlope)){
    configuration.CntSlope = 1.0;
    shouldSaveConfig = true;
  }
  if(isnan(configuration.CntOffset)){
    configuration.CntOffset = 0.0;
    shouldSaveConfig = true;
  }
  if(isnan(configuration.CntSeconds)){
    configuration.CntSeconds = 300;
    shouldSaveConfig = true;
  }
  Serial.print("CNT Slope: ");
  Serial.println(configuration.CntSlope, 6);
  Serial.print("CNT Offset: ");
  Serial.println(configuration.CntOffset, 6);
  #endif

  
  //Set to STATION mode on boot. May help with reconnect after self reset.
  WiFi.mode(WIFI_STA);

  Serial.println(F("----------------"));
  wifi_set_phy_mode(PHY_MODE_11N);
  WiFi.printDiag(Serial);
  WiFi.setOutputPower(20.5);
  Serial.println(F("----------------"));

  // id, placeholder, defaultvalue, length
  WiFiManagerParameter custom_shost("shost", "Host", shost, 64);
  WiFiManagerParameter custom_sport("sport", "Port", sport, 6);
  WiFiManagerParameter custom_sapikey("sapikey", "Key", sapikey, 32);
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);

  
  //reset settings - for testing
  //wifiManager.resetSettings();

  wifiManager.setAPStaticIPConfig(IPAddress(10,10,10,1), IPAddress(10,10,10,1), IPAddress(255,255,255,0));
  wifiManager.setConfigPortalTimeout(180);    //AP mode timeout after 3 minutes and try to connect to WiFi
  
  //add all your parameters here
  wifiManager.addParameter(&custom_shost);
  wifiManager.addParameter(&custom_sport);
  wifiManager.addParameter(&custom_sapikey);

  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //and goes into a 3 minute blocking loop awaiting configuration

  static char szSSID[20];
  sprintf(szSSID, "%s_%s", hwver.c_str(), MyMac.c_str());

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect(szSSID,"sensorhost")) {
    Serial.println(F("failed to connect, we should reset and see if it connects"));
    //Make sure these pins are inputs before reset or it may not boot
    pinMode (2, INPUT);
    pinMode (0, INPUT_PULLUP);
    pinMode (15, INPUT);

    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //read updated custom parameters and save to EEPROM
  if (shouldSaveConfig) {
    Serial.print(F("Saving config to EEPROM..."));
    store_config();
  }

  ledmode=2;
  //If we get this far, then we have connected to the WiFi access point
  Serial.print(F("Connected! Local IP: "));
  Serial.println(WiFi.localIP());
  Serial.println();

  startup();


  
}

// =============================================================
//##############################################################
void loop() {

  ts.update();    // Update the task scheduler
  yield();

  delay(1);

  recvWithEndMarker();    // Check for serial RX data
  showNewData();          // Process any received serial data

  #ifdef __HWVER_OLD__
    if (digitalRead(__PB) == 0){
      reset_wifi = true;
    }
  #endif
  if (reset_wifi == true){ resetToFactoryDefaults(); }
  if (reboot == true){ system_reboot(); }
  if (connect_counter > 9)
  {
    connect_counter - 0;
    reboot = true;
    printHeap();
    Serial.println(F("[HTTP] {ERR} Connect counter overflow."));
  }


  #ifdef _ENA_SWITCH
  if (send_switch == true){
    send_switch = false;
    String cs;
    if (current_state){
      cs = '1';
    } else {
      cs = '0';
    }

    tcp_send_temp("dat",cs,"OUT","SW1");
  }
  #endif

  #ifdef _ENA_CNTR
  readCNTR();     //Check the switch input and perform debounce
  yield();
  #endif

    
}

//##############################################################
// =============================================================
void sendRSSI(){
  String data = "cmd=rssi&id=" + dev_key + "&t1=" + String(WiFi.RSSI());
  post_data(data);

}

// =============================================================
void tcp_send_temp(String command, String value, String pre, String post){

  String data = "cmd=" + command + "&id=" + dev_key + "&t1=" + value + "&pre=" + pre + "&post=" + post;
  post_data(data);

}





//=====================================================================================
void store_config(){
  Serial.print(F("Writing config..."));
  EEPROM.put(2,configuration);
  EEPROM.commit();
  Serial.println(F("Done."));
  yield();
}
//=====================================================================================
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println(F("Entered config mode"));
  Serial.println(WiFi.softAPIP());
  //entered config mode, make led toggle faster
  ticker.attach(0.1, tick);
}
//=====================================================================================
#ifdef __HWVER_NEW__
void handleInterrupt() {
  detachInterrupt(digitalPinToInterrupt(__PB));
  reset_wifi = true;
}
#endif

//=====================================================================================
void resetToFactoryDefaults() {
  reset_wifi = false;
  Serial.println(F("[SYST] Factory Reset"));
  while (digitalRead(__PB)==0){
    for (uint8_t i=0;i<10;i++){
      digitalWrite(__DATALED, _DATALEDOFF);
      delay(50);
      digitalWrite(__DATALED, _DATALEDON);
      delay(50);
    }
  }
  digitalWrite(__DATALED, _DATALEDOFF);

  //Helps with reset, puts pins in specific state to allow reboot
  pinMode (2, INPUT_PULLUP);
  pinMode (0, INPUT_PULLUP);
  pinMode (15, INPUT);    //LOW on boot (relay output with 10k+100k pulldown)
    
  WiFi.disconnect();
  delay(1000);
  ESP.reset();
  delay(1000);
}


//=====================================================================================
void system_reboot() {
  Serial.println(F("[SYST] Reboot"));
  for (uint8_t i=0;i<10;i++){
    digitalWrite(__DATALED, _DATALEDOFF);
    delay(50);
    digitalWrite(__DATALED, _DATALEDON);
    delay(50);
  }
  digitalWrite(__DATALED, _DATALEDOFF);

  //Helps with reset, puts pins in specific state to allow reboot
  pinMode (2, INPUT_PULLUP);
  pinMode (0, INPUT_PULLUP);
  pinMode (15, INPUT);    //LOW on boot (relay output with 10k+100k pulldown)
    
  delay(1000);
  ESP.reset();
  delay(1000);
}




//=====================================================================================
void check_for_fw_update(uint8_t force){

  if (WiFi.status() != WL_CONNECTED){
    return;
  }
  yield();
  
  uint8_t old_ledmode = ledmode;
  ticker.attach(0.1, tick);   //Flash dataled FAST
  ledmode = 5;
  printHeap();
  Serial.print(F("[SYST] {FW } Update: "));
  
  t_httpUpdate_return ret = ESPhttpUpdate.update(update_server,fwver);
  yield();

  switch(ret) {
      case HTTP_UPDATE_FAILED:
          printHeap();
          Serial.printf(" failed, Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

      case HTTP_UPDATE_NO_UPDATES:
          printHeap();
          Serial.println(F("Up to date"));
          break;

      case HTTP_UPDATE_OK:
          printHeap();
          Serial.println(F("HTTP_UPDATE_OK"));
          break;
  } 
  ticker.detach();
  digitalWrite(__DATALED,_DATALEDOFF);       //Turn OFF LED
  ledmode = old_ledmode;
}








// =============================================================
void print_help(){
  Serial.println(F("################"));
  Serial.println(F("Sensorhost TCP"));
  Serial.println("HW Ver: " + hwver);
  Serial.println("FW Ver: " + fwver);
  Serial.println(update_server);
  Serial.println(F("----------------"));
  WiFi.printDiag(Serial);
  Serial.println(F("----------------"));
  Serial.print("FP: ");
  Serial.println(String(configuration.fingerprint));
  Serial.print(F("Host: "));
  Serial.println(configuration.shost);
  Serial.print(F("Port: "));
  Serial.println(configuration.sport);
  Serial.print(F("Api Key: "));
  Serial.println(configuration.sapikey);
  Serial.println(F("----------------"));
  #ifdef _ENA_TEMP
  Serial.print("TC1 Slope: ");
  Serial.println(configuration.tc1slope, 6);
  Serial.print("TC1 Offset: ");
  Serial.println(configuration.tc1offset, 6);
  #endif
  #ifdef _ENA_TEMP2
  Serial.print("TC2 Slope: ");
  Serial.println(configuration.tc2slope, 6);
  Serial.print("TC2 Offset: ");
  Serial.println(configuration.tc2offset, 6);
  #endif
  #ifdef _ENA_CNTR
  Serial.print("CNT Slope: ");
  Serial.println(configuration.CntSlope, 6);
  Serial.print("CNT Offset: ");
  Serial.println(configuration.CntOffset, 6);
  Serial.print("CNT Interval: ");
  Serial.print(configuration.CntSeconds);
  Serial.println(" secs");
  #endif
  Serial.println(F("################"));
  Serial.println();

}
  
// ************** END OF CODE *****************
