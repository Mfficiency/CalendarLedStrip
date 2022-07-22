#include "example_WifiLib.h"

WifiLib::WifiLib(bool displayMsg)
{
  // Anything you need when instantiating your object goes here
  _msg = displayMsg;
}

// Pretend this is one or more complex and involved functions you have written
const char *WifiLib::getSsid()
{
  return "***";
}

const char *WifiLib::getPass()
{
  return "***";
}

const char *WifiLib::getSite(int sitenr)
{
  if (sitenr == 1)
  {
    return "***"; // google cloud website
  }
  else
  {
    return "***"; // test website
  }
}
const int WifiLib::getMode()
{
  return 1;
}
const char *WifiLib::getToken()
{
  return "***";
}
