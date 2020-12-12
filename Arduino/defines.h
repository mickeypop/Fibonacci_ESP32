// ENABLES comment out to disable some functions as desired
  #define F_DEBUG                 // comment out to stop many debug printouts
  //#define DEBUG_ESP_HTTP_SERVER // turns on Server debugging, comment out when done
  //#define DFPLAYER_ENABLED      // comment out if you do not have the player
  //#define OLED_DEBUG            // oled display for debugging turn off if not used
  #define LIGHT_SENSOR            // LDR sense room lights out to dim, comment out if not used
// END ENABLES

// RGB led Header setup
  #define LED_SIGNAL_PIN     32   // Digital IO pin connected to the NeoPixels.
  #define PIXEL_COUNT         9   // Number of LEDs
  uint8_t LED_BRIGHTNESS  = 128;  // place holder for dimming option 
  uint8_t BRIGHTNESS      = 128;  // defaut brightness level
  uint8_t BRIGHTNESS_HOLD = LED_BRIGHTNESS; // used to track changes
// END RGB led Header setup

// DIMMING vars
  int BRIGHTNESS_ON_OFF   =   1;  // 0 will go full OFF with light out / 1 DIM based on BRITE_DIVISOR
  int BRITE_THRESHOLD     = 1600; // set for typical lighting, determine based on Your LDR value
                                  // just connect OHM meter to LDR and measure in different lighting
                                  // my LDR was around 1.0K room lite, partial lite 15K, 1M covered
                                  //  so ADC room light aprox     300
                                  //         partial light aprox 1400
                                  //         dark light aprox    2300
  int  BRITE_DIVISOR     =   35;  // sets how much dimming, LED_BRIGHTNESS/BRITE_DIVISOR  will be over riddedn from stored settings
  bool BRITE_DIM = true;          // LEAVE THIS, dim state tracking vars 
  bool BRITE_LAST = true;         // LEAVE THIS, placeholder when dim
// END DIMMING vars


// marks minutes between color change
// minutes ending in 0 or 5 all stay off
  #define LED_1_MINUTE_PIN  15  // minutes 1 & 6
  #define LED_2_MINUTE_PIN  13  // minutes 2 & 7
  #define LED_3_MINUTE_PIN  12  // minutes 3 & 8
  #define LED_4_MINUTE_PIN  14  // minutes 4 & 9

// pins
  #define MODE_BUTTON       33
  #define BRIGHTNESS_BUTTON 25

// DFPLAYER
  #define SERIAL1_RXPIN     16  // Only GPIO 12 gives problems
  #define SERIAL1_TXPIN     17
  #define BUSY_PIN          19  // see if sound play is done

// adc on ADC_0 io36,     Used by auto dimming when light out at nite
  #define ANALOG_PIN_0      36

// from original clock
  #define DEBOUNCE_DELAY    10
  #define MAX_BUTTONS_INPUT 20
  #define MAX_MODES          3
  #define MAX_PALETTES      10
  #define TOTAL_PALETTES    10
  #define CLOCK_PIXELS       5
