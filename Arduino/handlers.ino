// LIGHT sensing //
#ifdef LIGHT_SENSOR
  // LDR to GND with 10K pullup
  // if ADC0 > BRITE_THRESHOLD lights are out
  void setBrighDim()
  {
    int lite = analogRead(A0);
    //Serial.print("ADC0 = "); Serial.println(lite);  // turn on to find a threshhold point

    if(lite <= BRITE_THRESHOLD)
    {
      BRITE_DIM = true;  // lites ON
    } else
    {
      BRITE_DIM = false; // lites OFF
    }

    if( BRITE_DIM != BRITE_LAST  )  // did it change
    {
      if( BRITE_DIM == true )
      {  // lites on
        LED_BRIGHTNESS = BRIGHTNESS_HOLD;
        strip.setBrightness( LED_BRIGHTNESS );
      }else
      {  // lites OFF

        if( BRIGHTNESS_ON_OFF == 0 )
        {  // full OFF 
           LED_BRIGHTNESS = 0;
           strip.setBrightness( LED_BRIGHTNESS );
        } else
        {  // partial, dim
           LED_BRIGHTNESS = BRIGHTNESS_HOLD/BRITE_DIVISOR;
           strip.setBrightness( LED_BRIGHTNESS );
        }
      }
      setTimeNow();
      setMinuteLed();
    }
    BRITE_LAST = BRITE_DIM;
  }
#endif
// END LIGHT sensing //



void setTimeNow()  // called from web changes
{
  byte hours = hour( local )%12;
  byte minutes = minute(local);
  oldHours = hours;
  oldMinutes = minutes;

  for(int i=0; i<CLOCK_PIXELS; i++)
    bits[i] = 0;

  setBits(hours, 0x01);
  setBits(minutes/5, 0x02);

  for(int i=0; i<CLOCK_PIXELS; i++)
  {
    setPixel(i, colors[palette][bits[i]]);
    strip.show();
  }
}  // END setTimeNow()


void displayCurrentTime()
{
  setTime( hour( local )%12, minute(local)); 
} // END displayCurrentTime()

void setTime(byte hours, byte minutes)
{
  if(oldHours == hours && oldMinutes/5 == minutes/5)
    return;

  oldHours = hours;
  oldMinutes = minutes;

  for(int i=0; i<CLOCK_PIXELS; i++)
    bits[i] = 0;

  setBits(hours, 0x01);
  setBits(minutes/5, 0x02);

  for(int i=0; i<CLOCK_PIXELS; i++)
  {
    setPixel(i, colors[palette][bits[i]]);
    strip.show();
  }
}  // END setTime()


void setPixel(byte pixel, uint32_t color)
{
  if(!on)
    return;

  switch(pixel)
  {
    case 0:
      strip.setPixelColor(0, color);
      break;
    case 1:
      strip.setPixelColor(1, color);
      break;
    case 2:
      strip.setPixelColor(2, color);
      break;
    case 3:
      strip.setPixelColor(3, color);
      strip.setPixelColor(4, color);
      break;
    case 4:
      strip.setPixelColor(5, color);
      strip.setPixelColor(6, color);
      strip.setPixelColor(7, color);
      strip.setPixelColor(8, color);
      strip.setPixelColor(9, color);
      break;
  };
}  // END setPixel()


void rainbow(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j<256; j++) 
  {
    for(i=0; i< CLOCK_PIXELS; i++) 
    {
      setPixel(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);

    if( modeBtnPressed )
    {
      mode = (mode + 1)%MAX_MODES;
      resetButtonValues();
      return;
     modeBtnPressed = false; 
    }
    resetButtonValues();
  }
}  // END rainbow()


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) 
{
  uint16_t i, j;

  for(j=0; j < 256*5; j++) 
  {
    // 5 cycles of all colors on wheel
    for(i=0; i< CLOCK_PIXELS; i++) 
    {
      setPixel(i, Wheel(((i * 256 / CLOCK_PIXELS) + j) & 255));
    }
    strip.show();
    delay(wait);

// brightBtnPressed
// modeBtnPressed
    if( modeBtnPressed)
    {
      mode = (mode + 1)%MAX_MODES;
      resetButtonValues();
      return;
    }
    modeBtnPressed = false;
    resetButtonValues();
  }
}  // END rainbowCycle()


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) 
{
  if(WheelPos < 85) 
  {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) 
  {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else 
  {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}  // END Wheel()


void toggleOnOff()
{
  if( on )
  {
    for( int i=0; i<CLOCK_PIXELS; i++)
      setPixel(i, black);
    strip.show();
  }
  on = !on;

  if(on)
  {
    oldHours = 99;
    oldError = 99;
  }
}  // END toggleOnOff()


void setBits(byte value, byte offset)
{
  switch(value)
  {
    case 1:
      switch(random(2))
      {
        case 0:
          bits[0]|=offset;
          break;
        case 1:
          bits[1]|=offset;
          break;
      }
      break;
    case 2:
      switch(random(2))
      {
        case 0:
          bits[2]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[1]|=offset;
          break;
      }
      break;
    case 3:
      switch(random(3))
      {
        case 0:
          bits[3]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[2]|=offset;
          break;
        case 2:
          bits[1]|=offset;
          bits[2]|=offset;
          break;
      }
      break;
    case 4:
      switch(random(3))
      {
        case 0:
          bits[0]|=offset;
          bits[3]|=offset;
          break;
        case 1:
          bits[1]|=offset;
          bits[3]|=offset;
          break;
        case 2:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[2]|=offset;
          break;
      }
      break;
    case 5:
      switch(random(3))
      {
        case 0:
          bits[4]|=offset;
          break;
        case 1:
          bits[2]|=offset;
          bits[3]|=offset;
          break;
        case 2:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[3]|=offset;
          break;
      }
      break;
    case 6:
      switch(random(4))
      {
        case 0:
          bits[0]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[1]|=offset;
          bits[4]|=offset;
          break;
        case 2:
          bits[0]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          break;
        case 3:
          bits[1]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          break;
      }
      break;
    case 7:
      switch(random(3))
      {
        case 0:
          bits[2]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[4]|=offset;
          break;
        case 2:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          break;
      }
      break;
    case 8:
      switch(random(3))
      {
        case 0:
          bits[3]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[2]|=offset;
          bits[4]|=offset;
          break;
        case 2:
          bits[1]|=offset;
          bits[2]|=offset;
          bits[4]|=offset;
          break;
      }      
      break;
    case 9:
      switch(random(2))
      {
        case 0:
          bits[0]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[1]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;
          break;
      }      
      break;
    case 10:
      switch(random(2))
      {
        case 0:
          bits[2]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;
          break;
        case 1:
          bits[0]|=offset;
          bits[1]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;
          break;
      }            
      break;
    case 11:
      switch(random(2))
      {
        case 0:
          bits[0]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          bits[4]|=offset;      
          break;
        case 1:
          bits[1]|=offset;
          bits[2]|=offset;
          bits[3]|=offset;
          bits[4]|=offset; 
          break;
      }          
      break;
    case 12:
      bits[0]|=offset;
      bits[1]|=offset;
      bits[2]|=offset;
      bits[3]|=offset;
      bits[4]|=offset;        
      break;
  }
}  // END setBits()


void displayErrorCode()
{
  if(oldError == error)
    return;

  oldError = error;

  for(int i=0; i<CLOCK_PIXELS; i++)
    bits[i] = 0;

  if(error == 0)
  {
    setBits(12, 0x02);
  }
  else
  {
    setBits(error, 0x01);
  }
  for(int i=0; i<CLOCK_PIXELS; i++)
  {   
    setPixel(i, colors[palette][bits[i]]);
  }
  strip.show();
}  // END displayErrorCode()


void Brightness_buttonISR()
{
   if ( digitalRead( BRIGHTNESS_BUTTON ) == LOW )    // pressed btn
   {
       brightBtnPressed = true;
   }
}  // END Brightness_buttonISR()


// ///////////////
// MODE btn Pressed ISR
void  mode_buttonISR()
{
   if ( digitalRead( MODE_BUTTON ) == LOW )    // pressed btn
   {
       modeBtnPressed = true;
   }
}  // END mode_buttonISR()


// //////////
// Help Page
void handleHelp()
{ 
    server.send ( 200, "text/html", helppage );
}  // END handleHelp()


void handleRoot()
{ 
  if ( server.hasArg("timezone") ) 
  {
    handleSubmit();
  } else 
  {
    server.send ( 200, "text/html", getPage() );
  }  
}  // END handleRoot()


void handleSubmit()
{
  webTZ        = server.arg( "timezone" );
  webPallte    = server.arg( "pal" );
  Bright       = server.arg( "br" );
  String br    = server.arg( "S" );
  int S        = br.toInt();
  BRIGHTNESS   = Bright.toInt();

  //// photocell ////
  #ifdef LIGHT_SENSOR
    String OnOff = server.arg( "drad" );
    String Dtmp  = server.arg( "d" );  
  
    // set dimming options IF changed
    if( BRIGHTNESS_ON_OFF != OnOff.toInt() || BRITE_DIVISOR != Dtmp.toInt() )   // did it change?
    {
      if( OnOff == "0" )
      {
        BRIGHTNESS_ON_OFF = 0;
      } else
      {
        BRIGHTNESS_ON_OFF = 1;
        BRITE_DIVISOR     = Dtmp.toInt();
      }
      preferences.begin("USER_PREFS", false);
        preferences.putInt( "On_OFF", BRIGHTNESS_ON_OFF ); // dimming ON/OFF
        preferences.putInt( "Divisor", BRITE_DIVISOR );    // dimming divisor
      preferences.end();
      Serial.printf("\n\tOn/off %d Divisor %d", BRIGHTNESS_ON_OFF, BRITE_DIVISOR );
    }
  #endif

  //// Sound player ////
  #ifdef DFPLAYER_ENABLED
    String VOICE = server.arg( "voice" );
    String VOL   = server.arg( "vol" );   // read both to compare 
    String Vset  = server.arg( "V" );
    bool VOICE_CHANGE = false;

    //// VOICE ////
    if( PlayMode_Index != VOICE.toInt() )  // did it change?
    {
      VOICE_CHANGE = true;
      PlayMode_Index = VOICE.toInt();
    }

    //// Volume //// 
    if( DF_Volume != VOL || DF_Volume != Vset )   // did it change?
    {
      VOICE_CHANGE = true;
      DF_Volume = VOL;
      if( VOL.toInt() != Vset )  // allow hand entry on web page
      {
        DF_Volume = Vset;
      }  
      // set volume level here
       myDFPlayer.volume( DF_Volume.toInt() );  //Set new volume 
    }

    if( VOICE_CHANGE == true )    /// write change ??
    {
        preferences.begin("USER_PREFS", false);
          preferences.putInt( "PlayMode", PlayMode_Index ); // store PlayMode 
          preferences.putString( "DF_Volume", DF_Volume  ); // store volume 
        preferences.end();
    }    
  #endif   // END sound player


    Serial.printf( "\nTZone = %s\n", webTZ.c_str() );
    
  int dst_Test = TZRules[ webTZ.toInt() ][1].toInt();
  if( dst_Test == 1 )
  {
    setTZRules( 1 );  // DST supported
  } else
  {
    setTZRules( 0 );  // no DST
  }

  // changed palette?
  if( palette != webPallte.toInt() )  // has it changed
  {
    palette  = webPallte.toInt();

    // save it
    preferences.begin("TimeZone", false);
      preferences.putInt( "Palette", palette ); // palette
    preferences.end();
  }

  //// BRIGHTNESS  ////
  if( Bright.toInt() != S )  // allow hand entry on web page
  {
    BRIGHTNESS = S;
  } 
  
  // brightness changed?  changed but no save
  if( LED_BRIGHTNESS != BRIGHTNESS )
  {
     LED_BRIGHTNESS = BRIGHTNESS;
     BRIGHTNESS_HOLD = LED_BRIGHTNESS;   // SAVE TO SWITCH BRIGHT/DIM
     strip.setBrightness( LED_BRIGHTNESS );
         strip.show();
     setTimeNow();       // Update colors NOW
     preferences.begin("USER_PREFS", false);  //REF ONLY   PrefSSID = preferences.getString("ssid", "none"); 
        preferences.putInt( "U_Brightness", BRIGHTNESS  );
     preferences.end();
  }
  server.send( 200, "text/html", getPage() );
  //updateDisplay();
  //setTimeNow();
}  // END handleSubmit()


// TimeZone post from Android app  // send  http://192.168.1.212/tz?n=N
void handleTZ()
{
    webTZ = server.arg(0);
      Serial.printf("webTZ %s\n", webTZ.c_str() );
    if( isValidNumber( webTZ ) )  // is it a number
    {
      if( webTZ.toInt() >= 1 && webTZ.toInt() <= 46 )
      {
        // look up from index
        DST =  TZRules[ webTZ.toInt() ][1];  //

/*        if( DST == "1") // if 0 no dst, 1 has dst
        {
          setTZRules( 1 );
        } else
        {
          setTZRules( 0 );
        }
*/
          int dst_Test = TZRules[ webTZ.toInt() ][1].toInt();
          if( dst_Test == 1 )
          {
            setTZRules( 1 );  // DST supported
          } else
          {
            setTZRules( 0 );  // no DST
          }
      }
    }
  server.send ( 200, "", "" );
  //updateDisplay();
  //setTimeNow();
}  // END handleTZ()


// ID test GET from Android app  // send  http://192.168.1.212/fl?n=1
// if android app finds more than 1 clock
// flashes a few times to identify 
void handleIdFlash()
{
    String fl = server.arg(0);
      Serial.printf("fl= %s\n", fl.c_str() );
    if( isValidNumber( fl ) )  // is it a number
    {
      for( int i=0; i<4; i++)
      {
        //color RED .5 sec, BLUE .5 sec 
        colorWipe( strip.Color(255,0,0) , 1 ); // all RED 
        vTaskDelay( 500 );
        colorWipe( strip.Color(0,0,255) , 1 ); // all RED 
        vTaskDelay( 500 );
      }
      handleDisplay();    //put clock back to normal
      setTimeNow();       // re display
      setMinuteLed();     // turn on led for 1,2,3,4 and 6,7,8,9
    }
  server.send ( 200, "", "" );
}  // END handleIdFlash()


//  String webTZ
// String TZRules[47][2] on main tab bottom
//  { "1",  "-12,0"                 },   // -12:00 - Eniwetok, Kwajalein, InterNtl. Date Line
//  { "4",  "-9,1,3,2,1,2,11,1,1,2" },   // -9:00 DST Alaska
void setTZRules( int dst )   // lookup set and store rules
{
  int index = webTZ.toInt();
  webIndex = webTZ;
  String rules = TZRules[ index ][1];  // get rule by it's index

  TZ      = getValue( rules, ',', 0).toInt();
  DST     = getValue( rules, ',', 1);
  if( dst == 1  )   // DST supported
  {
    DTmonth = getValue( rules, ',', 2).toInt();
    DTwk    = getValue( rules, ',', 3).toInt();
    DTdow   = getValue( rules, ',', 4).toInt();
    DThr    = getValue( rules, ',', 5).toInt();
    STmonth = getValue( rules, ',', 6).toInt();
    STwk    = getValue( rules, ',', 7).toInt();
    STdow   = getValue( rules, ',', 8).toInt();
    SThr    = getValue( rules, ',', 9).toInt();
  } 
  saveTZ();               // put in preferences
  updateTimeZone();       // Sync time by the TZ
  updateDisplay();        // changed, update all now 
  setTimeNow();           // Update colors NOW
}  // END setTZRules()


// return current settings for Android
//   returns JSON  {"settings": {TZ:"-7",webIndex:"7",pallete:"0",brightness:"128",BRIGHTNESS_ON_OFF:"1","BRITE_DIVISOR":"15"} }
//              ,<BRITE_DIVISOR>,<Voice>,<Volume> depend on compile
void handleReturn()   // send  http://192.168.1.245/return
{
  String ret ="";
  //  ret ="\{\"settings\":";
    ret += "\{\"TZ\":\"";         ret += String(TZ);         ret += "\"," ;  //  TZ 
    ret += "\"webIndex\":\"";   ret += webIndex;           ret += "\"," ;  // get TZRules 
    ret += "\"pallete\":\"";    ret += String(palette);    ret += "\"," ;  // get Palette 
    ret += "\"brightness\":\""; ret += String(BRIGHTNESS); ret += "\"" ;     // get Brightness 
      #ifdef LIGHT_SENSOR
        ret += "," ;
        ret += "\"BRIGHTNESS_ON_OFF\":\""; ret += BRIGHTNESS_ON_OFF; ret += "\"," ;    // get dimming setting  0 OFF, 1 div brigntness/divisor
        ret += "\"BRITE_DIVISOR\":\"";     ret += String(BRITE_DIVISOR); ret += "\"" ;   // get dimming devisor
      #endif
      #ifdef DFPLAYER_ENABLED
        ret += "," ;
        ret += "\"PlayMode_Index\":\""; ret += String(PlayMode_Index); ret += "\"," ;   // get index to voice used
        ret += "\"DF_Volume\":\"";      ret += String(DF_Volume);      ret += "\"" ;      // get volume
      #endif
    ret += "\}";
   // ret += "\}";
  server.send ( 200, "", ret );

 String message = "\n";
  message += "URI: ";         message += server.uri();
  message += "\nMethod: ";    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: "; message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  Serial.println( message );
} // END handleReturn()


//// photocell ////
//  send  http://192.168.1.245/dim?on_off=N&div=N
//  on must be 0 or 1 ,  div 0 to 30
#ifdef LIGHT_SENSOR
  void handleDim() 
  {
    String ON_OFF  = server.arg( "on_off" );  // true/false returns 0 or 1, 0 OFF/1 DIM
    String DIVISOR = server.arg( "div" );

    if( isValidNumber(ON_OFF) )
    {
      if( BRITE_DIM != ON_OFF.toInt()  )  // did it change
      {
        BRITE_DIM = ON_OFF.toInt();
        preferences.begin("USER_PREFS", false);  // store it
           preferences.putBool( "U_Dim", BRITE_DIM  );
        preferences.end();
      }
    }

    if( isValidNumber(DIVISOR) )  // is it a number
    {
      if( DIVISOR.toInt() >= 0 && DIVISOR.toInt() <= 30 ) // is it in range?
      {
        if( BRITE_DIVISOR != DIVISOR.toInt() )  // has it changed
        {
            BRITE_DIVISOR  = DIVISOR.toInt();
            // save it
            preferences.begin("USER_PREFS", false);
               preferences.putInt( "Divisor", BRITE_DIVISOR );  // dimming divisor
            preferences.end();
        }
      }
    }
    server.send ( 200, "", "" );
  } // END handleDim()
#endif

// DONE
void handlePallete()       // send  http://192.168.1.212/p?n=N
{
  String ret1 = server.arg(0);
  if( isValidNumber(ret1) )  // is it a number
  {
    if( ret1.toInt() >= 0 && ret1.toInt() <= 9 ) // is it in range?
    {
      if( palette != ret1.toInt() )  // has it changed
      {
          palette  = ret1.toInt();
          // save it
          preferences.begin("TimeZone", false);
             preferences.putInt( "Palette", palette ); // palette
          preferences.end();
      }
      setTimeNow();           // Update colors NOW
      server.send ( 200, "", "" );
    }
  }
} // END handlePallete()


// DONE
void handleBrightness()     // send  http://192.168.1.212/b?n=N
{
  String ret1 = server.arg(0);
  if( isValidNumber(ret1) )  // is it a number
  {
    if( ret1.toInt() >= 25 && ret1.toInt() <= 255 )
    {
      BRIGHTNESS = ret1.toInt();
      // brightness changed?  changed but no save
      if( LED_BRIGHTNESS != BRIGHTNESS )  // did the brightness change
      {
        LED_BRIGHTNESS = BRIGHTNESS;
        BRIGHTNESS_HOLD = LED_BRIGHTNESS;   // SAVE TO SWITCH BRIGHT/DIM
        strip.setBrightness( LED_BRIGHTNESS );
        strip.show();
        setTimeNow();       // Update colors NOW
         preferences.begin("USER_PREFS", false);  //REF ONLY   PrefSSID = preferences.getString("ssid", "none"); 
           preferences.putInt( "U_Brightness", BRIGHTNESS  );
         preferences.end();
      }
    }
  }
  if(ret1 == "R")   // reset to standard Brightness
  {
      LED_BRIGHTNESS = 128;   // default
      BRIGHTNESS_HOLD = LED_BRIGHTNESS;   // SAVE TO SWITCH BRIGHT/DIM
      strip.setBrightness( LED_BRIGHTNESS );
      strip.show();
      setTimeNow();       // Update colors NOW
      preferences.begin("USER_PREFS", false);  //REF ONLY   PrefSSID = preferences.getString("ssid", "none"); 
         preferences.putInt( "U_Brightness", LED_BRIGHTNESS  );
      preferences.end();
  }
 server.send ( 200, "", "" );  // respond OK
}  // END handleBrightness()



// int  PlayMode_Index    = 0;    // 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
// int  MP3_fileIndex[]   = {0, 32, 38, 0, 16};  // starting index -1 to file order on SD card by mode 
// sayHI()  // announce audio change
void handleSound()        // send  http://192.168.1.212/s?n=N
{
 #ifdef DFPLAYER_ENABLED
  String ret1 = server.arg(0);
  if( isValidNumber(ret1) )  // is it a number
  {
    if( ret1.toInt() >= 0 && ret1.toInt() <= 4 ) // test if in range
    { 
      if( ret1.toInt() != PlayMode_Index )      // did it change ?
      {
        PlayMode_Index = ret1.toInt();
        sayHI();

        preferences.begin("USER_PREFS", false);  //REF ONLY   PrefSSID = preferences.getString("ssid", "none"); 
           preferences.putInt( "PlayMode", PlayMode_Index );  //for PlayMode_Index, 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
        preferences.end();
      }
    }
  }
  if(ret1 == "R")   // reset to Silent
  {
      PlayMode_Index    = 0;    // 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
  }
  sayHI();
  server.send ( 200, "", "" ); // answer back
 #endif
}  // END handleSound()


void handleVolume()     // send  http://192.168.1.212/v?n=N
{
  String ret1 = server.arg(0);
  if( isValidNumber(ret1) )  // is it a number
  {
    #ifdef DFPLAYER_ENABLED
    if( ret1.toInt() >= 0 && ret1.toInt() <= 30 )
    {

      if( ret1 != DF_Volume )  // did it change ?
      {
         DF_Volume = ret1;
         preferences.begin("USER_PREFS", false);  //REF ONLY   PrefSSID = preferences.getString("ssid", "none"); 
             preferences.putString( "DF_Volume", DF_Volume  );
         preferences.end();

         myDFPlayer.volume( DF_Volume.toInt() );  //Set new volume 
      }
    }
    #endif
  }
  if(ret1 == "R")   // reset to Silent
  {
    #ifdef DFPLAYER_ENABLED
      DF_Volume = "22";     // default bases on minium audible level
      preferences.begin("USER_PREFS", false);  //REF ONLY   PrefSSID = preferences.getString("ssid", "none"); 
         preferences.putString( "DF_Volume", DF_Volume  );
      preferences.end();

      // set volome level here
          myDFPlayer.volume( DF_Volume.toInt() );  //Set new volume 
    #endif
  }
  server.send ( 200, "", "" ); // answer back
}  // END handleVolume()


// builds the web page
String getPage()
{
  String page = "<html>\n<head><meta name=\"viewport\" content=\"initial-scale=1,maximum-scale=1,user-scalable=no\">\n";
  page += "<title>ESP32 Fibonacci Clock</title>\n";
  page += "\n<style>\nhtml,body{font-size:13px;padding:3px;margin:0;width:100%;line-height:2em;box-sizing:border-box;}\nsection{width:350px;margin:0 auto;}\nfieldset>legend{font-weight:bolder;}\nlabel>span,div>span{display:inline-block;margin-right:2px;}\ndiv.h{margin-left:22px;}\ndiv>span{width:52px;text-align:center;}\nlabel>span{width:50px;height:25px;border:1px solid black;vertical-align:bottom;}\n#l a{margin-left:5px;}table{font-size:13px;}\n#dbox{width:150px;}\n#S,#d,#V{width:30px;}\n.tab{margin-left: 2.5em}</style>\n\n";
  page += "\n</head>\n<body >\n";
  page += "\n<script language=\"JavaScript\">\n";
  page += "function updateSlider(val)\n";
  page += " { document.getElementById('S').value=val;}\n";
  page += "function updateVSlider(val)\n";
  page += " { document.getElementById('V').value=val;}\n";
  page += "</script>\n\n";
  page += "<section>\n";
  page += "<fieldset><legend>Clock Settings - Time: ";

  //// Header TimeZone stored ////
  page += String( hourFormat12( local) );   // hour
  page += ":";  
  page += String( minute( local) ); // minute
  page += " ";
    if ( isAM(local) )
    {
      page +=  "AM ";  // am/pm    
    } else
    {
      page +=  "PM ";  // am/pm    
    }
  page += "</legend>\n";
  
  page += "<form action='/' method='POST'>\n";
  page += "<table>\n<tr>\n <td colspan=\"5\"><span style=\"margin-right:1.15em; display:inline-block;\"></span>\n";
  page += "Current TimeZone: <b>";
  page +=  String(TZ);    // time zone 
  page += "</b> <span style=\"margin-right:1.25em; display:inline-block;\"></span> DST Used: <b>"; 

  // DST true/false
  if(DST == "1")
  {
    page += "Yes";
    page += "</b></td></tr>\n";  // end 1st table row
    page += "<tr>\n <td> </td><td></td><td>Month</td><td>Week</td><td>DoW</td><td>Hour</td></tr>\n";
    page += "<tr>\n <td><span style=\"margin-right:1.15em; display:inline-block;\"></span></td>\n";
    page += "<td>DST Start</td><td>";
      page += String( DTmonth );    //DST start Month
    page += "</td><td>";
      page += String( DTwk );    //DST start week
    page += "</td><td>";
      page += String( DTdow );    //DST start Day of week
    page += "</td><td>";
        page += String( DThr );   //DST start Hour
    page += "</td></tr>\n";
    page += "<tr>\n <td></td><td>DST End</td><td>";
        page += String( STmonth );     // ST end Month  
    page += "</td><td>";
        page += String( STwk );    // ST end week 
    page += "</td><td>";
        page += String( STdow );    // ST end Day of Week
    page += "</td><td>";
        page += String( SThr );    // ST end Hour
    page += "</td></tr>\n";
  } 
  if(DST == "0")
  {
    page += "No";
    page += "</b></td></tr>\n";  // end 1st table row
  }
  page += "</table>\n";

  //// TimeZone Selection ////
  page += "\n<b>TimeZone:</b> ";
  page += "\n<select name=\"timezone\" ID=\"dbox\">\n";
  page += " <option value=\"1\"";
    if( getValue( webIndex, ',', 0) == "1" ) page += " selected";
    page += ">-12:00 - Eniwetok, Kwajalein, InterNtl. Date Line</option>\n";
  page += " <option value=\"2\"";
    if( getValue( webIndex, ',', 0) == "2" ) page += " selected";
    page += ">-11:00 - Midway Island, Samoa</option>\n";
  page += " <option value=\"3\"";
    if( getValue( webIndex, ',', 0) == "3" ) page += " selected";
    page += ">-10:00 - Hawaii</option>\n";
  page += " <option value=\"4\"";
    if( getValue( webIndex, ',', 0) == "4" ) page += " selected";
    page += ">-9:00 DST Alaska</option>\n";
  page += " <option value=\"5\"";
    if( getValue( webIndex, ',', 0) == "5" ) page += " selected";
    page += ">-8:00 DST Tijuana, Baja California</option>\n";
  page += " <option value=\"6\"";
    if( getValue( webIndex, ',', 0) == "6" ) page += " selected";
    page += ">-8:00 DST Pacific Time (US &amp; Canada)</option>\n";
  page += " <option value=\"7\"";
    if( getValue( webIndex, ',', 0) == "7" ) page += " selected";
    page += ">-7:00 DST Mountain Time (US &amp; Canada)</option>\n";
  page += " <option value=\"8\"";
    if( getValue( webIndex, ',', 0) == "8" ) page += " selected";
    page += ">-7:00 - Arizona, Chihuahua, La Paz, Mazatlan</option>\n";
  page += " <option value=\"9\"";
    if( getValue( webIndex, ',', 0) == "9" ) page += " selected";
    page += ">-6:00 - Central America</option>\n";
  page += " <option value=\"10\"";
    if( getValue( webIndex, ',', 0) == "10" ) page += " selected";
    page += ">-6:00 DST Central Time (US &amp; Canada), Mexico City</option>\n";
  page += " <option value=\"11\"";
    if( getValue( webIndex, ',', 0) == "11" ) page += " selected";
    page += ">-6:00 DST Guadalajara, Mexico City, Saskatchewan, etc..</option>\n";
  page += " <option value=\"12\"";
    if( getValue( webIndex, ',', 0) == "12" ) page += " selected";
    page += ">-5:00 DST Eastern Time (US &amp; Canada)</option>\n";
  page += " <option value=\"13\"";
    if( getValue( webIndex, ',', 0) == "13" ) page += " selected";
    page += ">-5:00 - Bogota, Lima, Quito, Rio Branco</option>\n";
  page += " <option value=\"14\"";
    if( getValue( webIndex, ',', 0) == "14" ) page += " selected";
    page += ">-4:30 - Caracas</option>\n";
  page += " <option value=\"15\"";
    if( getValue( webIndex, ',', 0) == "15" ) page += " selected";
    page += ">-4:00 - Manaus, La Paz, Santiago</option>\n";
  page += " <option value=\"16\"";
    if( getValue( webIndex, ',', 0) == "16" ) page += " selected";
    page += ">-4:00 DST Atlantic Time (Canada)</option>\n";
  page += " <option value=\"17\"";
    if( getValue( webIndex, ',', 0) == "17" ) page += " selected";
    page += ">-3:30 * Newfoundland and Labrador</option>\n";
  page += " <option value=\"18\"";
    if( getValue( webIndex, ',', 0) == "18" ) page += " selected";
    page += ">-3:00 DST Brazil</option>\n";
  page += " <option value=\"19\"";
    if( getValue( webIndex, ',', 0) == "19" ) page += " selected";
    page += ">-3:00 DST Greenland, Buenos Aires, Georgetown,ontevideo/Uruguay</option>\n";
  page += " <option value=\"20\"";
    if( getValue( webIndex, ',', 0) == "20" ) page += " selected";
    page += ">-2:00 * Mid-Atlantic</option>\n";
  page += " <option value=\"21\"";
    if( getValue( webIndex, ',', 0) == "21" ) page += " selected";
    page += ">-1:00 DST Azores</option>\n";
  page += " <option value=\"22\"";
    if( getValue( webIndex, ',', 0) == "22" ) page += " selected";
    page += ">-1:00 - Cape Verde Islands</option>\n";
  page += " <option value=\"23\" ";
      if( webIndex == "0" || getValue( webIndex, ',', 0) == "23" ) page += " selected";
      page += ">0 GMT - Greenwich, London, Lisbon, Casablanca</option>\n";
  page += " <option value=\"24\"";
    if( getValue( webIndex, ',', 0) == "24" ) page += " selected";
    page += ">+1:00  DST Brussels, Copenhagen, Madrid, Paris</option>\n";
  page += " <option value=\"25\"";
    if( getValue( webIndex, ',', 0) == "25" ) page += " selected";
    page += ">+2:00 * Helsinki, Jerusalem, Cairo</option>\n";
  page += " <option value=\"26\"";
    if( getValue( webIndex, ',', 0) == "26" ) page += " selected";
    page += ">+2:00 DST Windhoek/ Africa, Namibia</option>\n";
  page += " <option value=\"27\"";
    if( getValue( webIndex, ',', 0) == "27" ) page += " selected";
    page += ">+3:00 - Minsk, Baghdad, Riyadh, Moscow, St. Petersburg</option>\n";
  page += " <option value=\"28\"";
    if( getValue( webIndex, ',', 0) == "28" ) page += " selected";
    page += ">+3:30 * Tehran</option>\n";
  page += " <option value=\"29\"";
    if( getValue( webIndex, ',', 0) == "29" ) page += " selected";
    page += ">+4:00 - Abu Dhabi, Muscat, Yerevan, Baku, Tbilisi</option>\n";
  page += " <option value=\"30\"";
    if( getValue( webIndex, ',', 0) == "30" ) page += " selected";
    page += ">+4:30 - Kabul</option>\n";
  page += " <option value=\"31\"";
    if( getValue( webIndex, ',', 0) == "31" ) page += " selected";
    page += ">+5:00 - Ekaterinburg, Islamabad, Karachi, Tashkent</option>\n";
  page += " <option value=\"32\""; 
    if( getValue( webIndex, ',', 0) == "32" ) page += " selected";
    page += ">+5:30 - Mumbai, Kolkata, Chennai, New Delhi</option>\n";
  page += " <option value=\"33\"";
    if( getValue( webIndex, ',', 0) == "33" ) page += " selected";
    page += ">+5:45 - Kathmandu</option>\n";
  page += " <option value=\"34\"";
    if( getValue( webIndex, ',', 0) == "34" ) page += " selected";
    page += ">+6:00 - Almaty, Dhaka, Novosibirsk,</option>\n";
  page += " <option value=\"35\"";
    if( getValue( webIndex, ',', 0) == "35" ) page += " selected";
    page += ">+6:30 - Yangon, Cocos Islands</option>\n";
  page += " <option value=\"36\"";
    if( getValue( webIndex, ',', 0) == "36" ) page += " selected";
    page += ">+7:00 - Bangkok, Hanoi, Jakarta</option>\n";
  page += " <option value=\"37\"";
    if( getValue( webIndex, ',', 0) == "37" ) page += " selected";
    page += ">+8:00 * Beijing, Perth, Singapore, Hong Kong</option>\n";
  page += " <option value=\"38\"";
    if( getValue( webIndex, ',', 0) == "38" ) page += " selected";
    page += ">+9:00 - Tokyo, Seoul, Osaka, Sapporo, Yakutsk</option>\n";
  page += " <option value=\"39\"";
    if( getValue( webIndex, ',', 0) == "39" ) page += " selected";
    page += ">+9:30 DST Adelaide, Darwin</option>\n";
  page += " <option value=\"40\"";
    if( getValue( webIndex, ',', 0) == "40" ) page += " selected";
    page += ">+10:00 - Brisbane Guam, Port Moresby, Vladivostok</option>\n";
  page += " <option value=\"41\"";
    if( getValue( webIndex, ',', 0) == "41" ) page += " selected";
    page += ">+10:00 DST Melbourne, Sydney, Australia</option>\n";
  page += " <option value=\"42\"";
    if( getValue( webIndex, ',', 0) == "42" ) page += " selected";
    page += ">+10:00 DST Hobart, Australia</option>\n";
  page += " <option value=\"43\"";
    if( getValue( webIndex, ',', 0) == "43" ) page += " selected";
    page += ">+11:00 - Magadan, Solomon Islands, New Caledonia</option>\n";
  page += " <option value=\"44\"";
    if( getValue( webIndex, ',', 0) == "44" ) page += " selected";
    page += ">+12:00 DST Auckland, Wellington</option>\n";
  page += " <option value=\"45\"";
    if( getValue( webIndex, ',', 0) == "45" ) page += " selected";
    page += ">+12:00 * Fiji, Kamchatka, Marshall Is.</option>\n";
  page += " <option value=\"46\"";
    if( getValue( webIndex, ',', 0) == "47" ) page += " selected";
    page += ">+13:00 - Nukualofa</option>\n";
  page += "</select>\n\n ";

  page += ", <a href=\"http://datasoftllc.com/TZ/tz.jpg\" target=\"_blank\" >TZ Map</a> ,\n ";
  page += "<a href=\"/help\" target=\"_blank\" >help</a><br>\n\n";

  /////  PALLETTE starts here /////
  page += "\n<hr>\n<b>Palettes:</b>\n\n<table>\n";
  page += "<tr><td></td><td></td><td>Hour</td><td>Minute</td><td>Both</td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"0\" ";
    if( webPallte == "0" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>RGB</td>\n <td style=\"background:#FF0A0A\"></td>\n <td style=\"background:#0AFF0A\"></td>\n <td style=\"background:#0A0AFF\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"1\" ";
    if( webPallte == "1" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>Mondrian</td>\n <td style=\"background:#FF0A0A\">\n </td><td style=\"background:#F8DE00\"></td>\n <td style=\"background:#0A0AFF\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"2\" ";
    if( webPallte == "2" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>Basbrun</td>\n <td style=\"background:#502800\">\n </td><td style=\"background:#14C814\"></td>\n <td style=\"background:#FF640A\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"3\" ";
    if( webPallte == "3" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>80's</td>\n <td style=\"background:#F564C9\"></td>\n <td style=\"background:#72F736\"></td>\n <td style=\"background:#71EBDB\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"4\" ";
    if( webPallte == "4" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>Pastel</td>\n <td style=\"background:#FF7B7B\">\n </td><td style=\"background:#8FFF70\"></td>\n <td style=\"background:#7878FF\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"5\" ";
    if( webPallte == "5" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>Modern</td>\n <td style=\"background:#70312D\">\n </td><td style=\"background:#91D231\"></td>\n <td style=\"background:#8D5FE0\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"6\" ";
    if( webPallte == "6" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>Cold</td>\n <td style=\"background:#D13EC8\"></td>\n <td style=\"background:#45E8E0\"></td>\n <td style=\"background:#5046CA\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"7\" ";
    if( webPallte == "7" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>Warm</td>\n <td style=\"background:#ED1414\"></td>\n <td style=\"background:#F6F336\"></td>\n <td style=\"background:#FF7E15\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"8\" ";
    if( webPallte == "8" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>Earth</td>\n <td style=\"background:#462300\"></td>\n <td style=\"background:#467A0A\"></td>\n <td style=\"background:#C8B600\"></td></tr>\n";
  page += "<tr>\n <td><input type=\"radio\" name=\"pal\" value=\"9\" ";
    if( webPallte == "9" ){  page += " checked=\"checked\" "; }
    page += "></td>\n <td>Dark</td>\n <td style=\"background:#D32222\"></td>\n <td style=\"background:#50974E\"></td>\n <td style=\"background:#101895\"></td></tr>\n";
  page += "</table>\n<hr>";

  //// BRIGHTNESS ////
  page += "<b>Brightness:</b>\n <input name=br type=range min=15 max=255 value=\"";
      page += String( BRIGHTNESS );  // was Bright,  updates from slider OS text entry now
      page += "\" onchange=\"updateSlider(this.value);\">\n <input type=\"text\" name=S id=\"S\" value=\"";
      page += String( BRIGHTNESS );  // was Bright,  updates from slider OS text entry now
      page += "\">\n\n<hr>\n";

  //// if the photocell option was enabled ////
  #ifdef LIGHT_SENSOR
     page += "<b>Dimming:</b>\n";
     page += "<input type=radio name=drad value=0 ";
        if( BRIGHTNESS_ON_OFF == 0 )  page += "checked";
     page += " >OFF / ";
     page += "\n<input type=radio name=drad value=1 ";
        if( BRIGHTNESS_ON_OFF == 1 )  page += "checked";
     page += " >Dim - Divisor ";
     page += "\n<input type=text name=d id=d value=\"";
     page += String( BRITE_DIVISOR );  // load from set value
     page += "\"><br>\n";
     page += "OFF turns full OFF when light out,<br>";
     page += "DIM Divides Brightness/Dim, 5 to 25 is usually best\n<hr>\n";
  #endif

   //// add voice here, if DF set, end with <hr> ////
  #ifdef DFPLAYER_ENABLED
     page += "<b>Chime\/ Voice: <\/b><br>\n";
     page += "<input type=radio name=voice value=0 ";
        if( PlayMode_Index == 0 )  page += "checked";
     page += " >Silent Mode<br>\n";
     page += "<input type=radio name=voice value=1 ";
        if( PlayMode_Index == 1 )  page += "checked";
     page += " >Westminster (long)<br>\n";
     page += "<input type=radio name=voice value=2 ";
        if( PlayMode_Index == 2 )  page += "checked";
     page += " >Westminster (short)<br>\n";
        page += ". Voices <br>\n";
        page += "<input type=radio name=voice class=tab value=3 ";
        if( PlayMode_Index == 3 )  page += "checked";
        page += " >Alice<br>\n";
        page += "<input type=radio name=voice class=tab value=4 ";
        if( PlayMode_Index == 4 )  page += "checked";
        page += " >George\n";
        page += "<hr>\n";

     //// VOLUME ////
     page += "<b>Volume:<\/b>\n <input name=vol type=range min=11 max=30 value=\"";
      page += DF_Volume;  // 
      page += "\" onchange=\"updateVSlider(this.value);\">\n <input type=\"text\" name=V id=\"V\" value=\"";
      page += DF_Volume;  // was volume,  updates from slider OS text entry now
      page += "\">\n\n<hr>\n";
   #endif

  //// SUBMIT button ////
  page += "<input type=\"submit\" value=\"Submit\">\n</fieldset>\n"; 
  page += "</form>\n</section>\n</body>\n</html>";
  return page;
}


// checks string back from web
bool isValidNumber(String str){
   for(byte i=0;i<str.length();i++)
   {
      if(isDigit(str.charAt(i))) return true;
   }
   return false;
} 


// REF var = getValue( StringVar, ',', 2); // if  a,4,D,r  would return D        
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length();

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}  // END getValue()


void resetButtonValues()
{
  for(int i=0; i<MAX_BUTTONS_INPUT; i++)
    lastButtonValue[i] = currentButtonValue[i];
}    // END resetButtonValues()


void checkErrors()
{
  error = 0;
  oldError = 99;
  mode = 3;
  palette = 0;

  int time1 = now();
  delay(1200);
  int time2 = now();
  if(time1 == time2)
  {
    error |= 0x02;  // time stopped ???
  }
}  // END checkErrors()


void readButtons()
{
  set_button = debounce( BRIGHTNESS_BUTTON );
  button     = debounce( MODE_BUTTON );
}   // END readButtons()

void handleButtons()
{
  if( brightBtnPressed )
  {
    Serial.println( "brightBtnPressed" );
    for(int i=0; i<100; i++)
    {
      if(!debounce(BRIGHTNESS_BUTTON) || !debounce(MODE_BUTTON))
        break;
    }
    if( debounce(BRIGHTNESS_BUTTON) && debounce(MODE_BUTTON))
    {
      checkErrors();
    }
    brightBtnPressed = false;
  }
  if( modeBtnPressed )
  {
    Serial.println( "modeBtnPressed" );
    mode = mode + 1;  // step thru modes

    if(mode >= MAX_MODES)
      mode = 0;
    modeBtnPressed = false;
  }
} // END handleButtons()


int debounce(int pin)
{
  int val = digitalRead(pin);
  if( val == lastButtonValue[pin] )
  {
    currentButtonValue[pin] = val;
    return val;
  }
  delay(DEBOUNCE_DELAY);
  
  val = digitalRead(pin);
  if( val != lastButtonValue[pin] )
  {
    currentButtonValue[pin] = val;
    return val;
  }
  currentButtonValue[pin] = lastButtonValue[pin];
  return lastButtonValue[pin];
}  // END debounce()
