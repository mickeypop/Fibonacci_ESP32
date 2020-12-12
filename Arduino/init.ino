void initButtons(void) 
{
  pinMode( BRIGHTNESS_BUTTON, INPUT_PULLUP);
  pinMode( MODE_BUTTON      , INPUT_PULLUP);

  attachInterrupt( digitalPinToInterrupt( BRIGHTNESS_BUTTON ), Brightness_buttonISR, FALLING ); // connect to ISR
  attachInterrupt( digitalPinToInterrupt( MODE_BUTTON )      , mode_buttonISR,       FALLING ); // connect to ISR
}

void initLEDs()
{
  pinMode( LED_1_MINUTE_PIN, OUTPUT );
  pinMode( LED_2_MINUTE_PIN, OUTPUT );
  pinMode( LED_3_MINUTE_PIN, OUTPUT );
  pinMode( LED_4_MINUTE_PIN, OUTPUT );

  all_LEDs_off();
  digitalWrite( LED_1_MINUTE_PIN, HIGH );
  delay(1500);
  all_LEDs_off();
  digitalWrite( LED_2_MINUTE_PIN, HIGH );
  delay(1500);
  all_LEDs_off();
  digitalWrite( LED_3_MINUTE_PIN, HIGH );
  delay(1500);
  all_LEDs_off();
  digitalWrite( LED_4_MINUTE_PIN, HIGH );
  delay(1500);
  
  all_LEDs_off();
}


void initUDP()
{
  Udp.begin(localPort);
  setSyncProvider( getNtpTime );    // callback to NTP time update

  checkNTPTimeValid();    // make sure NTP time is valid and/or fix
}

static void initServer(void *pvParameters)
{
  Serial.printf("Server Task: Started.\n");
  server.on( "/", handleRoot );         // human web page
  server.on( "/help", handleHelp );     // human help
  server.on( "/tz", handleTZ );         // android TimeZone
  server.on( "/pal", handlePallete );   // android Pallete
  server.on( "/br", handleBrightness ); // android Brightness
  server.on( "/fl", handleIdFlash );    // android flash for identification
  server.on( "/return", handleReturn ); // android return options set for configuring android screens
  
  //// photocell ////
  #ifdef LIGHT_SENSOR
    server.on( "/dim", handleDim );      // android Dimming, optional compile
  #endif
  #ifdef DFPLAYER_ENABLED
     server.on( "/sound", handleSound ); // android Sound choice, optional compile
     server.on( "/vol", handleVolume );  // android Sound Volume, optional compile
  #endif

  server.begin();
  Serial.println ( "HTTP Server started\n" );
  
  while ( 1 ) // run forever 
  {
    vTaskDelay( 500 ); // only delay 1 task not whole processor
    server.handleClient();
  }  
}  // END initServer()

// 2nd CORE 
void initServerTask()
{
  // Run at high priority to prevent data overruns.
  xTaskCreatePinnedToCore(
      initServer,             // Task function
      "Web Server",           // Printable name
      4096,                   // Stack size in words
      NULL,                   // Task parameter(s)
      4,                      // Priority
      NULL,                   // Task handle
      0);                     // 1 main core, 0 extra core 
  Serial.printf("Web Srver Task Setup: Complete.\n");
} // END startLaserTask()

void initVars()  // Load vars from NVR
{
  preferences.begin("USER_PREFS", false);
     DF_Volume = preferences.getString( "DF_Volume", "none" );  // DF_Volume
        if( DF_Volume == "none" ) DF_Volume = "22";  // fall back on default
     PlayMode_Index = preferences.getInt( "PlayMode", 0);  // 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
     LED_BRIGHTNESS = preferences.getInt( "U_Brightness" , 128 );     // brightness
     BRIGHTNESS_HOLD = LED_BRIGHTNESS;   // SAVE TO SWITCH BRIGHT/DIM
     
     // Dimming vars
     #ifdef LIGHT_SENSOR
        BRIGHTNESS_ON_OFF = preferences.getInt( "On_OFF", 0);   // dimming ON/OFF
        BRITE_DIM         = preferences.getBool( "U_Dim", true  );
        BRITE_DIVISOR     = preferences.getInt( "Divisor", 0);  // dimming divisor
     #endif 

     // Sound player
     #ifdef DFPLAYER_ENABLED
     #endif
  preferences.end();
}
