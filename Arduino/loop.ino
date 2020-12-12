void loop()
{
  if ( WiFi.status() == WL_CONNECTED )
  {
    // Main connected loop
   // analogValue = analogRead( ANALOG_PIN_0 ); // use for day/nite dimming
   // Serial.printf( "Pin value = %d \n", analogValue );

    checkNTPTimeValid();  // make sure NTP time is valid and/or fix
    clockUpdate();        // update local time from NTP
    handleDisplay();      // serial time & display routines

    ////  Fibonicci parts
    readButtons();
    handleButtons();

    resetButtonValues();    // Store buttons new values
    switch(mode)
    {
      case 0:
        displayCurrentTime();
        break;
      case 1:
        oldHours = 99;
        rainbowCycle(20);
        break;
      case 2:
        oldHours = 99;
        rainbow(20);
        break;
      case 3:
        oldHours = 99;
        displayErrorCode();        // Display error code
        break;
    }
    //// END Fibonicci parts

    #ifdef LIGHT_SENSOR
       setBrighDim();
    #endif
    delay(2000);
  }                 // END WiFi connected loop()
  else
  {                 // WiFi DOWN
    WFstatus = getWifiStatus( WFstatus );
    WiFi.begin( PrefSSID.c_str() , PrefPassword.c_str() );
    int WLcount = 0;
    while (WiFi.status() != WL_CONNECTED && WLcount < 200 )
    {
      delay( 100 );
      Serial.printf(".");

      if (UpCount >= 60)  // keep from scrolling sideways forever
      {
         UpCount = 0;
         Serial.printf("\n");
      }
      ++UpCount;
      ++WLcount;

        int temp = digitalRead(MODE_BUTTON);
      if (temp == LOW )
      {
        initSmartConfig();
      }
    }
    
   delay( 1000 );
  } // END WiFi down
} // END loop()
