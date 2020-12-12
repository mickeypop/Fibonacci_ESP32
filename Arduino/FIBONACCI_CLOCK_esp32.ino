
// ESP32 Fibonacci Clock
// https://github.com/mickeypop/Fibonacci_ESP32
//  has Android App too
#include "defines.h"           // many defined vars here
#include "FS.h"
#include "esp_system.h"
#include <esp_wifi.h>
#include <string.h>
#include <ctype.h> 
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>           // used to set fibonacci.local
#include <WebServer.h>         // https://github.com/bbx10/WebServer_tng

#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager replaced with zhouhan0126/WIFIMANAGER-ESP32

#include <Update.h>
#include <WiFiUdp.h>           // needed for NTP time connect
#include <TimeLib.h>           // NTP time
#include <Timezone.h>          // https://github.com/JChristensen/Timezone

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>              // I2C
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if ( SSD1306_LCDHEIGHT != 32)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#include <Preferences.h>       // WiFi/Clock settings storage
#include <Adafruit_NeoPixel.h> // RGB LEDs

//#### PROTO TYPES #### - Arduino compiler fix
void   updateDisplay();            // set LCD,check local, led for 1,2,3,4 and 6,7,8,9
bool   isValidNumber(String str);  // validate web return is number
String getValue(String data, char separator, int index); // parse TZRules
void   saveTZ();                   // store TZ settings
void   updateTimeZone();           // if TZ changes set all vars
void   setQuarterHourVar();        // set Chine counter
void   initPlayer();               // setup player
void   setMinuteLed();
void   resetButtonValues();
#ifdef OLED_DEBUG
  void   OLED_Display();           // OLED for debugging
#endif
//#####################

#ifdef DFPLAYER_ENABLED
  #include <DFRobotDFPlayerMini.h> // give it a voice https://github.com/DFRobot/DFRobotDFPlayerMini

  HardwareSerial Serial1(1);
  DFRobotDFPlayerMini myDFPlayer;
#endif
  String DF_Volume         = "22";
  int    PlayMode_Index    = 0;    // 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
  int    MP3_fileIndex[]   = {0, 32, 38, 0, 16};  // starting index to file order on SD card according to my mode
  int    quarterFlag       = 0;    // will flag 1/4 hour for WM chimes and ID chime to play
//### player end 


const char* rssiSSID;          // NO MORE hard coded set AP, all SmartConfig
const char* password;
String PrefSSID, PrefPassword; // used by preferences storage

WebServer server ( 80 );      // declear the server

// WiFi tatus vars
int WFstatus;
int UpCount = 0;
int32_t rssi;       // store WiFi signal strength here
String getSsid;     // WiFi setup
String getPass;     // WiFi setup
String MAC;
String MACfour;     // last 4 char of MAC address
String mDNS_Name = "Fibonacci";
String IPaddress;
String timeStamp;
String dateStamp;
int analogValue   = 0;


char PROGMEM timeServerURL[] = "time.nist.gov"; // server pool
IPAddress timeServer;             // ip address of server, leave blank DNS fills this in
int timeZone      = 0;            // GMT Time
WiFiUDP Udp;                      // NTP time connect setup
unsigned int localPort = 8888;    // local port to listen for UDP packets
char Time_Zone[4] = "0";          // replaced value from prefferences store

// TimeZone conversion VARs
// these will be set from your preferences setup 
String  DST     = "1"; // DST observed, 1 true, 0 false
uint8_t DTwk    = 2;   //DST start week
uint8_t DTdow   = 1;   //DST start Day of week
uint8_t DTmonth = 3;   //DST start Month
uint8_t DThr    = 2;   //DST start Hour
uint8_t STwk    = 1;   // ST start week
uint8_t STdow   = 1;   // ST start Day of Week
uint8_t STmonth = 11;  // ST start Month
uint8_t SThr    = 2;   // ST start Hour
int TZ          = -7;
int GMToffset   = TZ * 60;
int GMT_DST;

// TimeZone Rules 
TimeChangeRule myDT = { "DT" , DTwk, DTdow, DTmonth, DThr, GMT_DST   };  //UTC +- X-60 minutes Daylight Savings Time 
TimeChangeRule myST = { "ST" , STwk, STdow, STmonth, SThr, GMToffset };  //UTC +- X minutes    Standard Time
Timezone       myDST( myDT, myST );
TimeChangeRule *tcr;    //pointer to the time change rule, use to get the TZ abbrev
time_t local;
time_t utc;
// TimeZone Rules END

uint8_t lastMinute;
uint8_t lastHour; 

Preferences preferences;  // settings storage,  declare class object

// RGB led Header setup defines in defines.h
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, LED_SIGNAL_PIN, NEO_GRB + NEO_KHZ800);

// page vars
String helppage = "<html>\n<head>\n<meta name=\"viewport\" content=\"initial-scale=1,maximum-scale=1,user-scalable=no\">\n<style>\nhtml,body{font-size:13px;padding:3px;margin:0;width:100%;line-height:2em;box-sizing:border-box;}\nsection{width:350px;margin:0 auto;}\nfieldset>legend{font-weight:bolder;}\n#l a{margin-left:5px;}\n</style>\n</head>\n\n<body>\n<section>\n<fieldset>\n<legend>Clock Help</legend>\nSetup is easy<br>\nFirst know your TimeZone \n<a href=\"http://datasoftllc.com/images/tz.jpg\" target=\"_blank\" >TZ Map</a><br>\nDropdown info:<br><b> 'TimeZone\" \"DST Support\" \"City/ State\"</b><br>\n<table>\n</tr>\n<tr>\n  <td><span style=\"margin-right:1.15em; display:inline-block;\"></span></td>\n <td><b>DST</b></td>\n <td><span style=\"margin-right:0.65em; display:inline-block;\"></span></td>\n <td> if DST is used in TimeZone</td></tr>\n<tr> <td></td>\n <td><b>-</b></td>\n <td></td>\n <td>that TmeZone has no DST</td></tr>\n<tr>\n <td></td>\n <td><b>*</b></td>\n <td></td>\n <td>has DST but cannnot work with Library</td></tr>\n</table>\n\n DST will be automatic if selected.<br>\n<br>\nThough there are actually 47 time zones around the world, not all can work in the timezone library.\n</fieldset>\n</section>\n</body>\n</html>";
String Bright = "128";
String webPallte = "0";
String webTZ = "0";
String webIndex = "-";
String getPage();



////////////////////////////////
///// FIBONACCI STARTS HERE ////
////////////////////////////////
// from original
bool modeBtnPressed = false;    // modeBtnPressed
bool brightBtnPressed = false;  // brightBtnPressed

int set_button;
int button;

byte bits[CLOCK_PIXELS];

uint32_t black = strip.Color(0,0,0);
uint32_t colors[TOTAL_PALETTES][4] =  {
 { // #1 RGB
   strip.Color(255,255,255), // off
   strip.Color(255,10,10),   // hours   #FF0A0A
   strip.Color(10,255,10),   // minutes #0AFF0A
   strip.Color(10,10,255)    // both;   #0A0AFF
 }, 
 { // #2 Mondrian
   strip.Color(255,255,255), // off
   strip.Color(255,10,10),   // hours   #FF0A0A
   strip.Color(248,222,0),   // minutes #F8DE00
   strip.Color(10,10,255)    // both;   #0A0AFF
 }, 
 { // #3 Basbrun
   strip.Color(255,255,255), // off
   strip.Color(80,40,0),     // hours   #502800
   strip.Color(20,200,20),   // minutes #14C814
   strip.Color(255,100,10)   // both;   #FF640A
 },
 { // #4 80's
   strip.Color(255,255,255), // off
   strip.Color(245,100,201), // hours   #F564C9
   strip.Color(114,247,54),  // minutes #72F736
   strip.Color(113,235,219)  // both;   #71EBDB
 },
 { // #5 Pastel
   strip.Color(255,255,255), // off
   strip.Color(255,123,123), // hours   #FF7B7B
   strip.Color(143,255,112), // minutes #8FFF70
   strip.Color(120,120,255)  // both;   #7878FF
 },
 { // #6 Modern
   strip.Color(255,255,255), // off
   strip.Color(212,49,45),   // hours   #70312D
   strip.Color(145,210,49),  // minutes #91D231
   strip.Color(141,95,224)   // both;   #8D5FE0
 },
 { // #7 Cold
   strip.Color(255,255,255), // off
   strip.Color(209,62,200),  // hours   #D13EC8
   strip.Color(69,232,224),  // minutes #45E8E0
   strip.Color(80,70,202)    // both;   #5046CA
 },
 { // #8 Warm
   strip.Color(255,255,255), // off
   strip.Color(237,20,20),   // hours   #ED1414
   strip.Color(246,243,54),  // minutes #F6F336
   strip.Color(255,126,21)   // both;   #FF7E15
 },
 { //#9 Earth
   strip.Color(255,255,255), // off
   strip.Color(70,35,0),     // hours   #462300
   strip.Color(70,122,10),   // minutes #467A0A
   strip.Color(200,182,0)    // both;   #C8B600
 },
 { // #10 Dark
   strip.Color(255,255,255), // off
   strip.Color(211,34,34),   // hours   #D32222
   strip.Color(80,151,78),   // minutes #50974E
   strip.Color(16,24,149)    // both;   #101895
 }
}; 

  
bool on = true;

byte oldHours = 0;    // tracking, used to spot change 
byte oldMinutes = 0;

int lastButtonValue[ MAX_BUTTONS_INPUT ];
int currentButtonValue[ MAX_BUTTONS_INPUT ];
int mode = 0;
int palette = 0;

byte error = 0;
byte oldError = 0;

String PROGMEM TZRules[47][2] ={
 { "0",  "JUNK"                  },   // PLACE HOLDER, keep indexes off 0, constant
 { "1",  "-12,0"                 },   // -12:00 - Eniwetok, Kwajalein, InterNtl. Date Line
 { "2",  "-11,0"                 },   // -11:00 - Midway Island, Samoa
 { "3",  "-10,0"                 },   // -10:00 - Hawaii
 { "4",  "-9,1,3,2,1,2,11,1,1,2" },   // -9:00 DST Alaska
 { "5",  "-8,1,4,1,1,2,10,0,1,2" },   // -8:00 DST Tijuana, Baja California  
 { "6",  "-8,1,3,2,1,2,11,1,1,2" },   // -8:00 DST Pacific Time (US &amp; Canada)
 { "7",  "-7,1,3,2,1,2,11,1,1,2" },   // -7:00 DST Mountain Time (US &amp; Canada)
 { "8",  "-7,0"                  },   // -7:00 - Arizona, Chihuahua, La Paz, Mazatlan
 { "9",  "-6,0"                  },   // -6:00 - Central America
 {"10",  "-6,1,3,2,1,2,11,1,1,2" },   // -6:00 DST Central Time (US &amp; Canada), Mexico City
 {"11",  "-6,1,4,1,1,2,10,0,1,2" },   // -6:00 DST Guadalajara, Mexico City, Saskatchewan, etc..
 {"12",  "-5,1,3,2,1,2,11,1,1,2" },   // -5:00 DST Eastern Time (US &amp; Canada)
 {"13",  "-5,0"                  },   // -5:00 - Bogota, Lima, Quito, Rio Branco
 {"14",  "-4.5,0"                },   // -4:30 - Caracas
 {"15",  "-4,0"                  },   // -4:00 - Manaus, La Paz, Santiago
 {"16",  "-4,1,3,2,1,2,11,2,1,2" },   // -4:00 DST Atlantic Time (Canada)
 {"17",  "-3.5,0"                },   // -3:30 * Newfoundland and Labrador
 {"18",  "-3,1,10,3,1,0,2,3,1,0" },   // -3:00 DST Brazil
 {"19",  "-3,1,3,0,1,1,10,0,1,1" },   // -3:00 DST Greenland, Buenos Aires, Georgetown,ontevideo/Uruguay
 {"20",  "-2,0"                  },   // -2:00 * Mid-Atlantic
 {"21",  "-1,1,3,0,1,1,10,0,1,1" },   // -1:00 DST Azores
 {"22",  "-1,0"                  },   // -1:00 - Cape Verde Islands
 {"23",  "0,0"                   },   // 0 GMT - Greenwich, London, Lisbon, Casablanca
 {"24",  "1,1,3,0,1,1,10,0,1,1"  },   // +1:00  DST Brussels, Copenhagen, Madrid, Paris
 {"25",  "2,0"                   },   // +2:00 * Helsinki, Jerusalem, Cairo
 {"26",  "2,1,9,1,1,2,4,1,1,2"   },   // +2:00 DST Windhoek/ Africa, Namibia
 {"27",  "3,0"                   },   // +3:00 - Minsk, Baghdad, Riyadh, Moscow, St. Petersburg
 {"28",  "3.5,0"                 },   // +3:30 * Tehran
 {"29",  "4,0"                   },   // +4:00 - Abu Dhabi, Muscat, Yerevan, Baku, Tbilisi
 {"30",  "4.5,0"                 },   // +4:30 - Kabul
 {"31",  "5,0"                   },   // +5:00 - Ekaterinburg, Islamabad, Karachi, Tashkent
 {"32",  "5.5,0"                 },   // +5:30 - Mumbai, Kolkata, Chennai, New Delhi
 {"33",  "5.75,0"                },   // +5:45 - Kathmandu
 {"34",  "6,0"                   },   // +6:00 - Almaty, Dhaka, Novosibirsk,
 {"35",  "6.5,0"                 },   // +6:30 - Yangon, Cocos Islands
 {"36",  "7,0"                   },   // +7:00 - Bangkok, Hanoi, Jakarta
 {"37",  "8,0"                   },   // +8:00 * Beijing, Perth, Singapore, Hong Kong
 {"38",  "9,0"                   },   // +9:00 - Tokyo, Seoul, Osaka, Sapporo, Yakutsk
 {"39",  "9.5,1,10,1,1,2,4,1,1,2"},   // +9:30 DST Adelaide, Darwin
 {"40",  "10,0"                  },   // +10:00 - Brisbane Guam, Port Moresby, Vladivostok
 {"41",  "10,1,10,1,1,2,4,1,1,2" },   // +10:00 DST Melbourne, Sydney, Australia
 {"42",  "10,1,10,1,1,2,4,1,1,2" },   // +10:00 DST Hobart, Australia
 {"43",  "11,0"                  },   // +11:00 - Magadan, Solomon Islands, New Caledonia
 {"44",  "12,1,9,0,1,2,4,1,1,2"  },   // +12:00 DST Auckland, Wellington
 {"45",  "12,0"                  },   // +12:00 * Fiji, Kamchatka, Marshall Is.
 {"46",  "13,0"                  }    // +13:00 - Nukualofa
};
    
