void setTimeZone()
{
  preferences.begin("TimeZone", false);
    DST = preferences.getString("DST" , "NEW"  );   // DST observed, 1 true, 0 false
  preferences.end();
    if( DST == "NEW" )    // test if ever saved
    {
      DST = "1";
      saveTZ();  // 1 time initiallize Preferences
    }
   getTZ();

   setTZOffSet();
   buildTZ_Rules();
}

void updateTimeZone()
{
  saveTZ();       // save Preferences
  getTZ();
  setTZOffSet();
  buildTZ_Rules();
}

void buildTZ_Rules()
{
  if( DST == "1" )
  {
    myDST.setRules( myDT , myST );  // use DST
  } else
  {
    myDST.setRules( myST , myST ); // no DST used here
  }
  strcpy( Time_Zone, "0" );
  timeZone = atoi(Time_Zone); // CHAR to INT
}

void setTZOffSet()
{
  if( GMToffset == 0.0)   // GMT 0 numbers and correct
  {
      myDT.offset = GMToffset;
      #ifdef F_DEBUG
     //   Serial.printf( "0 myST.offset = %d\n  myDT.offset =   %d\n\n", myST.offset, myDT.offset  );
      #endif
  } else
  {
    if (GMToffset <= 0.0)  // Negative TimeZone number
    {
      myDT.offset = GMToffset + 60;
       #ifdef F_DEBUG
          Serial.printf( "From Preferences \n" );
          Serial.printf( "TZ = %d, DST = %s \n",  TZ , DST.c_str() );
     //   Serial.printf( "%d  %d \tmonth\n", myST.month, myDT.month  );
     //   Serial.printf( " %d  %d \tweek\n", myST.week , myDT.week  );
     //   Serial.printf( " %d  %d \tdow\n",  myST.dow  , myDT.dow  );
     //   Serial.printf( " %d  %d \thour\n", myST.hour , myDT.hour  );
     //   Serial.printf( " %d  %d offset\n\n", myST.offset, myDT.offset  );
          Serial.printf( "palette = %d\n\n",  palette );
      #endif
    } else
    {     // Positive TimeZone number
      myDT.offset = GMToffset - 60;
      #ifdef F_DEBUG
      //  Serial.printf( "> myST.offset = %d\n  myDT.offset =   %d\n\n", myST.offset, myDT.offset  );
      #endif
    }
  }
}


void saveTZ()
{
    preferences.begin("TimeZone", false);
    preferences.putString("webINDEX", webIndex  ); 
    preferences.putString("DST"     , DST  );    // DST observed, 1 true, 0 false
    preferences.putInt(   "TZ"      , TZ );      // TimeZone

    if( DST == "1")
    {
      preferences.putUChar( "DT_WK"   , DTwk );    //DST start week
      preferences.putUChar( "DT_DOW"  , DTdow );   //DST start Day of week
      preferences.putUChar( "DT_MONTH", DTmonth ); //DST start Month
      preferences.putUChar( "DT_HR"   , DThr );    //DST start Hour
      preferences.putUChar( "ST_WK"   , STwk );    // ST start week
      preferences.putUChar( "ST_DOW"  , STdow );   // ST start Day of Week
      preferences.putUChar( "ST_MONTH", STmonth ); // ST start Month
      preferences.putUChar( "ST_HR"   , SThr );    // ST start Hour       
    }
  preferences.end();
}

void getTZ()
{
 preferences.begin("TimeZone", false);
  webIndex = preferences.getString("webINDEX" , "1"  );
  DST      = preferences.getString("DST" , "1"  );   // DST observed, 1 true, 0 false
  TZ       = preferences.getInt(   "TZ"      , -5 ); // TimeZone
  if( DST == "1")
  {
    DTmonth = preferences.getUChar( "DT_MONTH", 3 );  //DST start Month
    DTwk    = preferences.getUChar( "DT_WK"   , 2 );  //DST start week
    DTdow   = preferences.getUChar( "DT_DOW"  , 1 );  //DST start Day of week
    DThr    = preferences.getUChar( "DT_HR"   , 2 );  //DST start Hour
    STmonth = preferences.getUChar( "ST_MONTH", 11 ); // ST start Month
    STwk    = preferences.getUChar( "ST_WK"   , 1 );  // ST start week
    STdow   = preferences.getUChar( "ST_DOW"  , 1 );  // ST start Day of Week
    SThr    = preferences.getUChar( "ST_HR"   , 2 );  // ST start Hour
  }  
  palette = preferences.getInt(  "Palette"  , 0 ); // palette
 preferences.end();

 GMToffset = TZ * 60; // account for timezone hours
}
