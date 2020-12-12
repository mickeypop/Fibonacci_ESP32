// here is where to find notes
//
// though i used preferences.h lib, locking in the ESP32,  
// if the references were replaced with SPIFFS it could work on ESP8266

// defines.h has most defined vars 
//   BRIGHTNESS_ON_OFF - 0 will go full OFF with light out/ 1 DIM based on BRITE_DIVISOR
//              so if BRIGHTNESS is set to say 128 
//                    dim is set to say 10%,  led will go to 12 since brigtness is an int   

// smartConfig needs Android or IOS app from Esspressif
//     Android, this app is available under name “IOT_Espressif” or another application “ESP8266 SmartConfig” (this is for ESP8266 but you can use it for ESP32):
//                  https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch
//     IOS      this app is available under name “Espressif Esptouch”:
//                  https://itunes.apple.com/us/app/espressif-esptouch/id1071176700?mt=8

// DODO's
// - take buttons off interrupt
// - get all Modes working
// - activated Brighness button (once off interrupt)
// 
// - fix Sound compile issues
//
// - DONE -- make Dim/OFF setable
// - DONE -- HIDE Sound/voice if turned off in compile
// 
// 
