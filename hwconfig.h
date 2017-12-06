/**
 * Include file for hardware settings
 * 
 * ===========================
 * Available UART commands
 * ---------------------------
 * ## Single word commands ##
 * help             Prints info about the sensor
 * resetwifi        Clears wifi credentials and reboots sensor
 * resetsensors     Sets the slope to 1.0 and offset to 0.0 for all sensors
 * reboot           Just reboot the device
 * 
 * ## Commands that require parameters
 * port=        0-65535 currently 80
 * host=        server address such as sensorhost.com
 * apikey=      API Key provided for the sensor (SHGT21XLB22)
 * tc1slope=    Slope (multiplier for sensor)
 * tc1offset=   Offset (addition for sensor)
 * tc2slope=    Slope (multiplier for sensor)
 * tc2offset=   Offset (addition for sensor)
 * cnt1slope=   Slope (multiplier for sensor)
 * cnt1offset=  Offset (addition for sensor)
 * cnt1seconds= Interval in seconds between sending counts
 * 
 * ssid=        New SSID to use
 * psk=         New PSK to use
 * restartwifi  Reboot sensor and use new credentials
 * 
 * 
 */

// =============================================================
// COMPILER CONDITIONALS - Set ONE ONLY for the hardware variant


//#define __sh11__     /* ONE Temp Sensor OLD HW */
#define __sh12__     /* ONE Temp Sensor NEW HW */
//#define __sh21__     /* ONE Temp Sensor, ONE Switch OLD HW */
//#define __sh22__     /* ONE Temp Sensor, ONE Switch NEW HW */
//#define __sh31__     /* TWO Temp Sensor OLD HW */
//#define __sh32__     /* TWO Temp Sensor NEW HW */
//#define __sh41__     /* TWO Temp Sensor, ONE Switch - OLD HW */
//#define __sh42__     /* TWO Temp Sensor, ONE Switch - NEW HW */
//#define __sh51__     /* ONE Counter OLD HW */
//#define __sh52__     /* ONE Counter NEW HW */



// RELEASED FIRMWARE
/**
 * ENSURE YOU UPDATE THE BUILD VERSION IN THE SELECTED SENSOR BELOW
 * 
 **/



/* ONE Temp Sensor */
#ifdef __sh11__
  #define __HWVER_OLD__
  #define _ENA_TEMP
  String hver = "11";
  String hwver = "sh" + hver;
  String build = "2017112102.bin";
#endif
#ifdef __sh12__
  #define __HWVER_NEW__
  #define _ENA_TEMP
  String hver = "12";
  String hwver = "sh" + hver;
  String build = "2017120601.bin";
#endif

/* ONE Temp Sensor, ONE Switch */
#ifdef __sh21__
  #define __HWVER_OLD__
  #define _ENA_TEMP
  #define _ENA_SWITCH
  String hver = "21";
  String hwver = "sh" + hver;
String build = "2017112102.bin";
#endif
#ifdef __sh22__
  #define __HWVER_NEW__
  #define _ENA_TEMP
  #define _ENA_SWITCH
  String hver = "22";
  String hwver = "sh" + hver;
#endif

/* TWO Temp Sensor */
#ifdef __sh31__
  #define __HWVER_OLD__
  #define _ENA_TEMP
  #define _ENA_TEMP2
  String hver = "31";
  String hwver = "sh" + hver;
  String build = "2017112102.bin";
#endif
#ifdef __sh32__
  #define __HWVER_NEW__
  #define _ENA_TEMP
  #define _ENA_TEMP2
  String hver = "32";
  String hwver = "sh" + hver;
  String build = "2017112102.bin";
#endif

/* TWO Temp Sensor */
#ifdef __sh41__
  #define __HWVER_OLD__
  #define _ENA_TEMP
  #define _ENA_TEMP2
  #define _ENA_SWITCH
  String hver = "41";
  String hwver = "sh" + hver;
#endif
#ifdef __sh42__
  #define __HWVER_NEW__
  #define _ENA_TEMP
  #define _ENA_TEMP2
  #define _ENA_SWITCH
  String hver = "42";
  String hwver = "sh" + hver;
#endif


/* ONE COUNTER Sensor */
#ifdef __sh51__
  #define __HWVER_OLD__
  #define _ENA_CNTR
  String hver = "51";
  String hwver = "sh" + hver;
  String build = "2017112102.bin";
#endif
#ifdef __sh52__
  #define __HWVER_NEW__
  #define _ENA_CNTR
  String hver = "52";
  String hwver = "sh" + hver;
  String build = "2017112102.bin";
#endif




String fwver = hver + build;
String update_server = "http://sensorhost.com/firmware/http" + hwver + "/update.php";


#ifdef __USE_PDO
String post_file = "/direct/shdirhttp1.php";
#else
String post_file = "/direct/shxwrapidat1.php";
#endif


#ifdef __HWVER_OLD__
  #define __PB 0        //(D3) Push button
  #define __STATUSLED 5   //(D1)
  #define __DATALED 16    //NodeMCU board RED LED
  #define _STATUSLEDON HIGH
  #define _STATUSLEDOFF LOW
  #ifdef _ENA_SWITCH
    #define __SWIN 13       //(D7)
  #endif
  #ifdef _ENA_CNTR
    #define __SWIN 13       //(D7)
  #endif
  #ifdef _ENA_TEMP
    #include <OneWire.h>
    #include <DallasTemperature.h>
    #define __DStcA 4        //(D2) DS18B20 One Wire Bus
    OneWire oneWireA(__DStcA);
    DallasTemperature DS18B20A(&oneWireA);
  #endif
  #ifdef _ENA_TEMP2
    #define __DStcB 12        //(D6) DS18B20 One Wire Bus
    OneWire oneWireB(__DStcB);
    DallasTemperature DS18B20B(&oneWireB);
  #endif
#endif

#ifdef __HWVER_NEW__
  #define __PB 5        //(D1) Push button
  #define __STATUSLED 16
  #define __DATALED 0    //NodeMCU board RED LED
  #define _STATUSLEDON LOW
  #define _STATUSLEDOFF HIGH
  #ifdef _ENA_SWITCH
    #define __SWIN 13       //(D7)
  #endif
  #ifdef _ENA_CNTR
    #define __SWIN 13       //(D7)
  #endif
  #ifdef _ENA_TEMP
    #include <OneWire.h>
    #include <DallasTemperature.h>
    #define __DStcA 4        //(D2) DS18B20 One Wire Bus
    OneWire oneWireA(__DStcA);
    DallasTemperature DS18B20A(&oneWireA);
  #endif
  #ifdef _ENA_TEMP2
    #define __DStcB 12        //(D6) DS18B20 One Wire Bus
    OneWire oneWireB(__DStcB);
    DallasTemperature DS18B20B(&oneWireB);
  #endif
#endif


#define _DATALEDON LOW
#define _DATALEDOFF HIGH



/***************************************************
 * Pinout
 * D0  = 16 = LED_BUILTIN
 * D1  =  5 = LED1
 * D2  =  4 = DS18B20
 * D3  =  0 = __PB (Push button input pullup) [HIGH on boot]
 * D4  =  2 = [HIGH on boot]
 * D5  = 14 = 
 * D6  = 12 = 
 * D7  = 13 = DIGIN
 * D8  = 15 = RELAY OUT [LOW on boot]
 * D9  = 3  =
 * D10 = 1  = TXD0
 * 
 * The items in [brackets] are the required levels on
 *  those pins on power up to boot normally.
 * 
 ****************************************************/

