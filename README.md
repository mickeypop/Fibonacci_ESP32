# Fibonacci Clock with ESP32 WiFi
ESP32 Atomic Fibonacci Clock with Android App and full Daylight Savings Time(DST) support

Unlike other Fibonacci Clocks, this is fully Atomic.  
Just set WiFi and a few presets with the first Arduino preset sketch to set preferances then compile the main sketch.

I also gave it minute accuracy, most Fibonacci Clocks can only show within 5 minutes.  4 Extra LEDs place the minutes that do not end in 0 or 5.

I added I2C LCD support for debugging but is not needed in use.

With mDNS support you can access it by web page or Android app as **fibonacci.local** once it is on your WiFi network.

Just choose your Time Zone and the DST rule that applies to you and the clock time is set.

Because I used the preferances library this will not work on the ESP8266 but with the Preferances replaced with file system and say JSON this could be adapted easily.
