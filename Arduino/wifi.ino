void wifiInit() //
{
  Serial.printf("\n WiFi Setup \n " );
  WiFi.mode(WIFI_AP_STA);    // required to read NVR before WiFi.begin()

  // load credentials from NVR, a little RTOS code here
  wifi_config_t conf;
  esp_wifi_get_config(WIFI_IF_STA, &conf);     // load wifi settings to struct comf
  rssiSSID = reinterpret_cast<const char*>(conf.sta.ssid);
  password = reinterpret_cast<const char*>(conf.sta.password);

  //  Serial.printf( "SSID = %s\n", rssiSSID );  // un-comment for debuging
  //  Serial.printf( "Pass = %s\n", password );  // un-comment for debuging
  // Open Preferences with wifi namespace. Namespace is limited to 15 chars
  preferences.begin("wifi", false);
    PrefSSID     = preferences.getString("ssid", "none");     //NVS key ssid
    PrefPassword = preferences.getString("password", "none"); //NVS key password
  preferences.end();

  // keep from rewriting flash if not needed
  if( !checkPrefsStore() )   // see is NV and Prefs are the same
  {                          // not the same, setup with SmartConfig
    if( PrefSSID == "none" ) // New...setup wifi
    {
      initSmartConfig();     // WiFi was never set yet
      delay( 3000);
      ESP.restart();         // reboot with wifi configured
    }
  }
  WiFi.begin( PrefSSID.c_str() , PrefPassword.c_str() );

  int WLcount = 0;
  while (WiFi.status() != WL_CONNECTED && WLcount < 200 ) // can take > 100 loops depending on router settings
  {
    delay( 50 );
    Serial.printf(".");
    ++WLcount;
  }
  Serial.printf("\n");
  delay( 500 );
} // END wifiInit()


void saveWifiSSID()
{
  preferences.begin("wifi", false); // put it in storage
    preferences.putString( "ssid" ,    getSsid);
    preferences.putString( "password", getPass);
  preferences.end();
}


// match WiFi IDs in NVS to Pref store, assumes WiFi.mode(WIFI_AP_STA); was executed
bool checkPrefsStore()
{
  bool val = false;
  String NVssid, NVpass, prefssid, prefpass;

  NVssid = getSsidPass( "ssid" );
  NVpass = getSsidPass( "pass" );

  // Open Preferences with my-app namespace. Namespace name is limited to 15 chars
  preferences.begin("wifi", false);
    prefssid  =  preferences.getString("ssid",     "none"); //NVS key ssid
    prefpass  =  preferences.getString("password", "none"); //NVS key password
      
  preferences.end();

  if( NVssid.equals(prefssid) && NVpass.equals(prefpass) )
  { 
    val = true; 
  }
  return val;
} // END checkPrefsStore()


void initSmartConfig()
{
  int loopCounter = 0;
  WiFi.mode( WIFI_AP_STA );     //Init WiFi, start SmartConfig

///////////////
  #ifdef OLED_DEBUG
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10,1);
    display.print( "WiFi Setup" );      
    display.setCursor( 10,13);
    display.print( "SmatConfig" );
    display.display();
  #endif
///////////////

  WiFi.beginSmartConfig();

  while (!WiFi.smartConfigDone())
  { 
    Serial.printf( "." );
    if( loopCounter >= 60 )  // just keep from scrolling off the side 
    {
      loopCounter = 0;
      Serial.printf( "\n" );
    }
    delay(600);
    ++loopCounter;
  }
  loopCounter = 0;  //smartConfig done clear var

  Serial.printf("\nSmartConfig received.\n Waiting for WiFi\n\n");
  delay(2000 );

  while( WiFi.status() != WL_CONNECTED )
  {    // check till connected
     delay(500);
  }
  IP_info();  // print connection info

  saveWifiSSID();
  delay(300);
} // END SmartConfig()


void initWPS()
{
  
}

// both buttons pressed on boot loads AP to set WiFi credentials
void initWiFiManager()
{
  WiFi.mode( WIFI_AP_STA );
    WiFiManager wifiManager;    //WiFiManager Local intialization. 
    //wifiManager.resetSettings();    //reset settings - for testing

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    //wifiManager.setTimeout(120);

    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    
    if (!wifiManager.startConfigPortal("Fib_Clock_AP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
//      ESP.restart();      //reset and try again, or maybe put it to deep sleep
//      delay(5000);
    }

    //if you get here you have connected to the WiFi
    IP_info();  // print connection info

  saveWifiSSID();
  delay(300);
}


void IP_info()
{
  getSsid = WiFi.SSID();
  getPass = WiFi.psk();
  rssi = getRSSI( rssiSSID );
  WFstatus = getWifiStatus( WFstatus );
  MAC = getMacAddress();
  MACfour = getMacFour();

  Serial.printf( "\n SSID\t%s \n", getSsid.c_str() );
//  Serial.print( rssi);  Serial.printf(" dBm\n" );  // printf??
  Serial.printf( " Pass:\t%s\n", getPass.c_str() ); 
  Serial.print( "\n IP address:\t" );  Serial.print(WiFi.localIP() );
  Serial.print( " / " );              Serial.println( WiFi.subnetMask() );
  Serial.print( " Gateway IP:\t" );  Serial.println( WiFi.gatewayIP() );
  Serial.print( " 1st DNS:\t" );     Serial.println( WiFi.dnsIP() );
  Serial.printf( " MAC:\t\t%s\n\n", MAC.c_str() );
}  // END IP_info()

int getWifiStatus( int WiFiStatus )
{
  WiFiStatus = WiFi.status();
  Serial.printf(" Status %d", WiFiStatus );
  switch( WiFiStatus )
  {
    case WL_IDLE_STATUS : // WL_IDLE_STATUS = 0,
        Serial.printf(", WiFi IDLE \n");
        break;
    case WL_NO_SSID_AVAIL: // WL_NO_SSID_AVAIL = 1,
        Serial.printf(", NO SSID AVAIL \n");
        break;
    case WL_SCAN_COMPLETED: // WL_SCAN_COMPLETED = 2,
        Serial.printf(", WiFi SCAN_COMPLETED \n");
        break;
    case WL_CONNECTED: // WL_CONNECTED = 3,
        Serial.printf(", WiFi CONNECTED \n");
        break;
    case WL_CONNECT_FAILED: // WL_CONNECT_FAILED = 4,
        Serial.printf(", WiFi WL_CONNECT FAILED\n");
        break;
    case WL_CONNECTION_LOST: // WL_CONNECTION_LOST = 5,
        Serial.printf(", WiFi CONNECTION LOST\n");
        WiFi.persistent(false); // don't write FLASH
        break;
    case WL_DISCONNECTED: // WL_DISCONNECTED = 6
        Serial.printf(", WiFi DISCONNECTED ==\n");
        WiFi.persistent(false); // don't write FLASH when reconnecting
        break;
  }
  return WiFiStatus;
}  // END getWifiStatus()

// Get the station interface MAC address.
String getMacAddress(void)  // @return Global String MAC
{
  WiFi.mode(WIFI_AP_STA);   // required to read NVR before WiFi.begin()
  uint8_t baseMac[6];
  esp_read_mac( baseMac, ESP_MAC_WIFI_STA );  // Get MAC address for WiFi station
  char macStr[18] = { 0 };                    // clear the string
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String( macStr );
}  // END getMacAddress()


// Get the station interface MAC address last 4 places fore mDNS ID name.
String getMacFour(void)  // @return Global String MAC
{
 // WiFi.mode(WIFI_AP_STA);   // required to read NVR before WiFi.begin()
  uint8_t baseMac[6];
  esp_read_mac( baseMac, ESP_MAC_WIFI_STA ); // Get MAC address for WiFi station
  char macStr[18] = { 0 };    // clear the string
  sprintf(macStr, "%02X%02X", baseMac[4], baseMac[5]);
  return String( macStr );
}  // END getMacFour()



// Return RSSI or 0 if target SSID not found
// call: int32_t rssi = getRSSI(SSID);
int32_t getRSSI( const char* target_ssid )
{
  byte available_networks = WiFi.scanNetworks();  // build a list

  for (int network = 0; network < available_networks; network++)
  {
    if ( strcmp( WiFi.SSID( network).c_str(), target_ssid ) == 0)
    {
      return WiFi.RSSI( network );
    }
  }
  return 0;
} // END getRSSI()


// Requires; #include <esp_wifi.h>
// Returns String NONE, ssid or pass arcording to request
String getSsidPass( String s )   // ie String var = getSsidPass( "pass" );
{
  String val = "NONE";    // return "NONE" if wrong key sent
  s.toUpperCase();
  if( s.compareTo("SSID") == 0 )
  {
    wifi_config_t conf;
    esp_wifi_get_config( WIFI_IF_STA, &conf );
    val = String( reinterpret_cast<const char*>(conf.sta.ssid) );
  }
  if( s.compareTo("PASS") == 0 )
  {
    wifi_config_t conf;
    esp_wifi_get_config( WIFI_IF_STA, &conf );
    val = String( reinterpret_cast<const char*>(conf.sta.password) );
  }
  return val;
}  // END getSsidPass()


// /*-------- NTP code ----------*/
void sPrintI00( int val )
{    if (val < 10) Serial.print('0');
    Serial.print(val, DEC);
    return;
}

void sPrintDigits( int val )
{   Serial.print(':');
    if(val < 10) Serial.print('0');
    Serial.print(val, DEC);
}

void printTime( time_t t ) 
{
  printDigits   ( month(t) );  Serial.print( '/' );
  sPrintI00     ( day(t) );  Serial.print( '/' );
  Serial.print  ( year(t) );
  Serial.print( " Time: " );
  sPrintI00( hourFormat12(t) );
  sPrintDigits  ( minute(t)  );
  sPrintDigits( second(t) );
  Serial.println( );
}
/////////////////

void printDigits(int digits)       // utility: pads colon and leading 0
{
  if(digits < 10)
     Serial.printf( "0" );
  Serial.printf( "%d", digits);
}

  const int NTP_PACKET_SIZE = 48;     // NTP time is in the first 48 bytes of message
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ;   // discard any previously received packets
  sendNTPpacket( timeServer );
  uint32_t beginWait = millis();
  while (millis() - beginWait < 2000) 
  {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) 
     {
       Udp.read(packetBuffer, NTP_PACKET_SIZE);    // read packet into the buffer
       unsigned long secsSince1900;
       // convert four bytes starting at location 40 to a long integer
       secsSince1900  = (unsigned long)packetBuffer[40] << 24;
       secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
       secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
       secsSince1900 |= (unsigned long)packetBuffer[43];
       return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
     }
  }
     Serial.printf("No NTP Response :-(\n");
  return 0;     // return 0 if unable to get the time
}  // END getNtpTime()


// send an NTP request to the time server at the given address
void sendNTPpacket( IPAddress &address )
{   // resolve DNS of server pool
  WiFi.hostByName( timeServerURL , address );
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket( address, 123 );    //NTP requests are to port 123
  Udp.write( packetBuffer, NTP_PACKET_SIZE );
  Udp.endPacket();
}   // END sendNTPpacket()


void checkNTPTimeValid()  // make sure NTP time is valid and/or fix
{
  if ( year() != 1970 )                // UDP good?, set 1hr re-sync
  {
     setSyncInterval( 3600 );          // Set the number of seconds between re-sync, 1Hr
  }
  if ( year() == 1970 )                // did udp fail?, do it again  minutes
  {
     setSyncInterval( 60 );            // Set the number of seconds between re-sync, 1Hr
     setSyncProvider( getNtpTime );    // callback to NTP time update
  }
}

void clockUpdate()
{
    utc = now();
    local = myDST.toLocal(utc, &tcr);
}

// /*-------- END NTP code ----------*/
