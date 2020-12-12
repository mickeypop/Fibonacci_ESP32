void initPlayer()
{
  pinMode( BUSY_PIN, INPUT_PULLUP );

  // DF_PLAYER serial ﻿
  #ifdef DFPLAYER_ENABLED
    delay(5000);
    Serial1.begin(9600, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);

    if ( !myDFPlayer.begin( Serial1, false ) ) 
    {
      Serial.println( myDFPlayer.readType(),HEX );
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while(true);
    }
    myDFPlayer.reset();
    delay( 600 );
    Serial.println(F("DFPlayer Mini online."));
  
    myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms

    //----Pre Sets 
    myDFPlayer.volume(22);      //Set volume value (0~30).
    myDFPlayer.volumeUp();      //Volume Up
    myDFPlayer.volumeDown();    //Volume Down
  
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    myDFPlayer.outputDevice( DFPLAYER_DEVICE_SD );

    int delayms=100;

    //----Read imformation----
    Serial.print(F("readState =\t"));
    Serial.println(myDFPlayer.readState()); //read mp3 state
    Serial.print(F("readVolume =\t"));
    Serial.println( myDFPlayer.readVolume()); //read current volume
    //Serial.print(F("readEQ =\t"));
    //Serial.println(myDFPlayer.readEQ()); //read EQ setting
    Serial.print(F("readFileCounts =\t"));
    Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
    Serial.print(F("readCurrentFileNumber =\t"));
    Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    Serial.print(F("readFileCountsInFolder =\t"));
    Serial.println( myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
    Serial.println(F("--------------------"));

    Serial.println(F("myDFPlayer.play(1)"));
    myDFPlayer.play(1);  //Play the first mp3

  #endif
}


// play audio as called for
// int    PlayMode_Index    = 0;    // 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
// int    MP3_fileIndex[]   = {0, 32, 38, 0, 16};  // starting index to file order on SD card my mode
void SoundPlay()      // called from handleDisplay() if minute = 0,15,30, or 45
{
  switch( PlayMode_Index ) 
  {
    case 0:                 // silece
      setQuarterHourVar();  // quarterFlag
      break;
    case 1:                 // WM long
      WestMinster();  
      break;
    case 2:                 // WM short
      WestMinster();  
      break;
    case 3:                 // alice
      humanVoice();
      break;
    case 4:                 // george
      humanVoice();
      break;
  }
}


void setQuarterHourVar()  // set quarterFlag at boot and hour change
{
  uint8_t startMinute = minute();
  if( startMinute >=  0 && startMinute <= 14 ) quarterFlag = 1;
  if( startMinute >= 15 && startMinute <= 29 ) quarterFlag = 2;
  if( startMinute >= 30 && startMinute <= 44 ) quarterFlag = 3;
  if( startMinute >= 45 && startMinute <= 59 ) quarterFlag = 0;
}

void playClockSound( int mp3_file)
{
  #ifdef DFPLAYER_ENABLED
     myDFPlayer.play( mp3_file );  //Play mp3
  #endif
}

// int  PlayMode_Index    = 0;    // 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
// int  MP3_fileIndex[]   = {0, 32, 38, 0, 16};  // starting index to file order on SD card my mode
void WestMinster( )   // 0 WM long, 1 WM short
{
#ifdef DFPLAYER_ENABLED
  uint8_t thisMinute = minute();
  uint8_t thisHour = hour( local);
  int BaseINDEX = MP3_fileIndex[ PlayMode_Index ]; // First WM file # -1

  if( thisMinute  == 15 && quarterFlag == 1)       // 1/4 hour
  {
    myDFPlayer.play( BaseINDEX + 1 );             // play 0001.mp3
    quarterFlag = 2;
  }
  if( thisMinute == 30 && quarterFlag == 2)        // 1/2 hour
  {
    myDFPlayer.play( BaseINDEX + 2 );             // play 0002.mp3
    quarterFlag = 3;
  }
  if( thisMinute == 45 && quarterFlag == 3)       // 3/4 hour
  {
    myDFPlayer.play( BaseINDEX + 3 );             // play 0003.mp3
    quarterFlag = 0;
  }
  if( thisMinute == 0 && quarterFlag == 0)        // hour
  {
    myDFPlayer.play( BaseINDEX + 4 );             // play 0004.mp3 hour start chime
    quarterFlag = 1;
    
//    while( isPlayerBusy() )  // pause if playr not done
//    {
//       vTaskDelay( 50 ); // only delay 1 task not whole processor
//    }
  
    if (thisHour >= 2) 
    {
      if (thisHour >= 13)           // adjust hour count
      {
        thisHour = thisHour - 12;
      }
      if (thisHour == 0)            // adjust for midnite
      {
        thisHour = 12;              // at midnight do 12 chimes
      }
      for ( int i = 1; i <= thisHour - 1; i++) 
      {
        myDFPlayer.play( BaseINDEX + 5 );       // play 0005.mp3, chime without end tail, hour -1 times

//        while( isPlayerBusy() )     // pause if playr not done
//        {
//          vTaskDelay( 70 );         // only delay 1 task not whole processor
//        }
      }
    }
    vTaskDelay( 80 );                     // only delay 1 task not whole processor
    myDFPlayer.play( BaseINDEX + 6 );     // play 0006.mp3, chime with long tail sound
  }
#endif
}

bool isPlayerBusy()  // true if busy, BUSY pin low when busy
{
  bool ret = false;
  if( digitalRead( BUSY_PIN ) == LOW  )
  {
     ret = true;
  } 
  return ret;
}


// int  PlayMode_Index    = 0;    // 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
// int  MP3_fileIndex[]   = {0, 32, 38, 0, 16};  // starting index -1 to file order on SD card by mode 
void humanVoice()           // 0 Alice, 16 George
{
#ifdef DFPLAYER_ENABLED
  uint8_t thisMinute = minute();
  uint8_t thisHour = hour( local);
  int BaseINDEX = MP3_fileIndex[ PlayMode_Index ]; // First file # -1  Alice

  if( thisMinute == 0 && quarterFlag == 0)         // hour
  {
    if( thisHour >= 1 && thisHour <= 11  )
    {
      myDFPlayer.play( BaseINDEX + thisHour );     // play 
    } else
    {
      if ( isAM(local) )
      {
        myDFPlayer.play( BaseINDEX + 13 );  // 12 "its 12 o'clock",  13 "it Mid Nite"
      } else
      {
        myDFPlayer.play( BaseINDEX + 14 );  // 12 "its 12 o'clock",  14 "it's Noon"
      }
    }
  } 
  setQuarterHourVar();     // quarterFlag
#endif
}

// int  PlayMode_Index    = 0;    // 0 silent, 1 WM long, 2 WM short, 3 Alice, 4 George
// int  MP3_fileIndex[]   = {0, 32, 38, 0, 16};  // starting index -1 to file order on SD card by mode 
void sayHI()  // announce audio change
{
 #ifdef DFPLAYER_ENABLED
   int BaseINDEX = MP3_fileIndex[ PlayMode_Index ];
   if( PlayMode_Index != 0 )
   {
     if( PlayMode_Index < 3 )
     {
       myDFPlayer.play( BaseINDEX + 4 );  // hour
     } else
     {
       myDFPlayer.play( BaseINDEX + 16 ); // "Hi im" alice or George accordingly
    }
   } else
   {
     myDFPlayer.play( BaseINDEX + 15 );   // 15 Alice, "Voices off"  31 for george
   }
 #endif
}
