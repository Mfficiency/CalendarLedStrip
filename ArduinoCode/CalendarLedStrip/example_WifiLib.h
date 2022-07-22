#ifndef wl
#define wl

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class WifiLib  {
  public:
    // Constructor 
    WifiLib(bool displayMsg=false);

    // Methods
    const char* getSsid();
    const char* getPass();
    const char* getSite(int sitenr);
    const int getMode();
    const char* getToken();
    
  private:
    bool _msg;
};
#endif
