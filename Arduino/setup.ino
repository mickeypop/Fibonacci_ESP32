void setup() 
{
  Serial.begin( 115200 );
  Serial.println( "\nSetup() starting\n\n" );
    delay(2000);

  initVars();  // load saved  presets

  #ifdef DFPLAYER_ENABLED
     initPlayer();  // setup MP3 player
     myDFPlayer.volume( DF_Volume.toInt() );  //Set volume value (0~30).  
  #endif

  #ifdef OLED_DEBUG
     bootDisplay(); // start oled
     Serial.println( "bootDisplay done!" );
  #endif

  RGB_Setup();    // register RGB led to class
  initLEDs();     // minute marking leds
  initButtons();  //

int bootModeBTN = debounce( MODE_BUTTON );
int bootBrightBTN = debounce( BRIGHTNESS_BUTTON );

  // hold MODE_BUTTON at boot to setup SmartConfig WiFi
  if( bootModeBTN == LOW && bootBrightBTN == HIGH ) 
  {
     colorWipe( strip.Color(255,0,0) , 1 ); // all RED 
     initSmartConfig();                       // first time to load SmartConfig WiFi Data
     colorWipe( strip.Color(0,0,0) , 1 );     // all OFF
     modeBtnPressed = false;
     brightBtnPressed = false;
  }

  // hold BRIGHTNESS_BUTTON at boot to setup WPS WiFi
  if( bootModeBTN == HIGH && bootBrightBTN == LOW ) 
  {
     colorWipe( strip.Color(0,255,0) , 1 ); // all GREEN
     initWPS();           // first time to load WPS WiFi Data
     colorWipe( strip.Color(0,0,0) , 1 );     // all OFF
     modeBtnPressed = false;
     brightBtnPressed = false;
  }

  // hold both BRIGHTNESS_BUTTON and MODE_BUTTON at boot to setup WifiManager WiFi
  if( bootModeBTN == LOW && bootBrightBTN == LOW ) 
  {
     colorWipe( strip.Color( 0,0,255) , 1 ); // all BLUE
     initWiFiManager();           // load WiFiManager WiFi Data
     colorWipe( strip.Color(0,0,0) , 1 );     // all OFF
     modeBtnPressed = false;
     brightBtnPressed = false;
  }


  colorWipe( strip.Color(175,175,0) , 1 ); // all YELLOW indicating WiFi setup is happening

  wifiInit(); // get WiFi connected
  IP_info();

  initUDP();   // setup internet time
  setTimeZone();
  checkNTPTimeValid();  // make sure NTP time is valid and/or fix
  randomSeed( ( random( millis() ) * second( local) ) / 13 ); 

  mDNS_Name += MACfour;  // build mDNS name

  Serial.printf( "\n  nDNS_Name:\t%s\n\n", mDNS_Name.c_str() );
  
  // set mDNS name, if fails flash YELLOW till rebooted  
  if( !MDNS.begin( mDNS_Name.c_str() ) )
  {
    Serial.println("Error setting up MDNS!");
    while(1)  // freeze , flash RED forever ???
    {
       colorWipe( strip.Color(255,0,0) , 1 ); // all RED 
       delay(1000);
       colorWipe( strip.Color(0,0,0) , 1 ); // all OFF
       delay(500);
    }
  }
   MDNS.addService("http", "tcp", 80);  // mDNS for android app

  initServerTask();   // start the web server on 2nd core

  delay(200);

  oldHours = 99;      // Make sure the time is always displayed the first time the clock is powered on.

  setQuarterHourVar();  // quarterFlag for use with chimes
} // END setup()
