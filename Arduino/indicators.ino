void handleDisplay()
{
  if( lastMinute != minute()  )
  {
   // printTime( local );   // Serial.print every minute TimeZone adjusted from NTP time
    OLED_Display();
    delay( 1000 );
    displayCurrentTime();  // RGB leds
    setMinuteLed();        // turn on led for 1,2,3,4 and 6,7,8,9

    if( minute() == 0 || minute() == 15 || minute() == 30 || minute() == 45 )
    {
      SoundPlay();  // call audio here
    }
    
    if( minute() % 5 == 0  )  // print on 5 minut increaments
    {
      printTime( local );   // Serial.print TimeZone adjusted from NTP time
    }
  }
 lastMinute = minute();
 }


void updateDisplay()
{
    updateTimeZone();
    printTime( local );    // Serial.print every minute TimeZone adjusted from NTP time
    OLED_Display();
    delay( 1000);
    setMinuteLed();        // turn on led for 1,2,3,4 and 6,7,8,9 nimut times
}

void RGB_Setup()
{
  strip.begin(); 
  BRIGHTNESS_HOLD = LED_BRIGHTNESS;   // SAVE TO SWITCH BRIGHT/DIM
  strip.setBrightness( LED_BRIGHTNESS ); 
  colorWipe( strip.Color(0,0,0) , 1 );     // all OFF
}

void colorWipe(uint32_t c, uint8_t wait)   // Fill the dots one after the other with a color
{
   for( uint16_t i=0; i<strip.numPixels(); i++ ) 
   {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
   }
}  // END colorWipe( )


///// OLED Display //// was just for debugging not final use
void OLED_Display() 
{
  IPaddress = WiFi.localIP().toString();
  BuildTimeString();
  #ifdef OLED_DEBUG
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10,1);
  display.print( IPaddress );      
  display.setCursor( 10,13);
  display.print( dateStamp );      
  display.setCursor( 10,25);
  display.print( timeStamp );      
  display.display();
  #endif
} // END OLED_Display()


void all_LEDs_off()  // minute markers off
{
  digitalWrite( LED_1_MINUTE_PIN, LOW );
  digitalWrite( LED_2_MINUTE_PIN, LOW );
  digitalWrite( LED_3_MINUTE_PIN, LOW );
  digitalWrite( LED_4_MINUTE_PIN, LOW );
}  // END

void setMinuteLed()  // 4 leds tell minuts between 0 -5 and 5-10's place column
{
  //setBrighDim();  // dim on minute if room lights go out
  
  int ones = 0;
  int val = minute();
  ones = val % 10;     // strip off 10s column from number
  all_LEDs_off();      // turn all off ans start fresh

  if( BRITE_DIM == true )
  {
      if ( ones == 1 || ones == 6 )
      {
        digitalWrite( LED_1_MINUTE_PIN, HIGH );
      }
      if ( ones == 2 || ones == 7 )
      {
        digitalWrite( LED_2_MINUTE_PIN, HIGH );
      }
      if ( ones == 3 || ones == 8 )
      {
        digitalWrite( LED_3_MINUTE_PIN, HIGH );
      }
      if ( ones == 4 || ones == 9 )
      {
        digitalWrite( LED_4_MINUTE_PIN, HIGH );
      }
  }
}  // END setMinuteLed()


void bootDisplay()
{
  Wire.begin( SDA, SCL  );  // start OLED 
    Serial.println( "Wire.begin done!" );

  #ifdef OLED_DEBUG  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin( SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
    Serial.println( "display.begin done!" );
  display.clearDisplay();

  display.setTextSize(1);  // text display
  display.setTextColor(WHITE);
  display.setCursor(10,1);
  display.println("Fibonicci Clock");

  display.setCursor(10,13);
  display.println("NTP Atomic Time");

  display.setCursor(10,25);
  display.println("by NTP Protocol");
  
  display.display();
  #endif
} // END bootDisplay()


//  needs TimeLib.h and NTC setup
//  REQUIRES; external global Strings "timeStamp" and "dateStamp"
void BuildTimeString()
{
  timeStamp = "";   // clear it for new value
  dateStamp = "";   // clear it for new value

  dateStamp = String( month( local ) , DEC) + "/";  

   if( day() < 10)   // pad "0" if single digit
      dateStamp = dateStamp + "0";
  dateStamp = dateStamp + String( day( local ) , DEC) + "/"; 
  dateStamp = dateStamp + String( year( local ) , DEC);
  timeStamp = String( hourFormat12( local ) , DEC) + ":";

    if( minute() < 10)   // pad "0" if single digit
      timeStamp = timeStamp + "0";
  timeStamp = timeStamp + String( minute() , DEC);
   
   if ( isAM(local) )
   {
      timeStamp = timeStamp + " AM";
   } else
   {
      timeStamp = timeStamp + " PM";
   }
}  // END buildTimeStamp()
