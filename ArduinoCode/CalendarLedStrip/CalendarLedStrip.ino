/*
Current working version of the ledstrip

Search for TODO to find parameters to change before use

Search for "button" to find the various button related code additions.

You can view the serial monitor to see a message when the button is pressed.


This uses JChristensen's Button Library from: 
https://github.com/JChristensen/JC_Button

*/

#include "WifiLib.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#include <FastLED.h>
#include "JC_Button.h"    // Include Button library

// JC button setup and params ------------------------------------------------
const uint8_t buttonPin = 4;  // = GPIO4 = D2 Set digital pin used with debounced pushbutton // TODO
Button myButton(buttonPin, true, true, 50);  // Declare the button

// Fastled setup and params -------------------------------------------------------

#define LED_PIN    0 // =GPIO0 =  // TODO
#define LED_TYPE    WS2812B // TODO
#define COLOR_ORDER GRB // TODO
#define NUM_LEDS    144 // TODO
#define BRIGHTNESS          255 // 

// TODO Enable this if you want to use RGB LEDs ex: WS2812B
CRGB leds[NUM_LEDS];
CRGB prevleds[NUM_LEDS];

// TODO Enable this if you want to use RGBW LEDs ex: SK6812
/*
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];
CRGBW prevleds[NUM_LEDS];
//*/

// Wifi setup and params ---------------------------------------------------
WifiLib tel(true); // TODO use your own wifi credentials library
WiFiClient wifiClient;

const char *ssid = tel.getSsid(); // TODO name of your wifi network
const char *password = tel.getPass(); // TODO wifi password
const char *website1 = tel.getSite(1); // TODO url of 
const char *website2 = tel.getSite(2);
const int mode = tel.getMode();
const char *token = tel.getToken();

String currWebsite = String(website1);
String callWebsite = String(website1);
String useToken = String(token);
String useAction = "Calendar";

double refreshRate = 6; // how many times per minute does a webcall need to go out

// Different modes --------------------------------------------------------
int useMode = mode;
/*
  1 : show set of leds slowly
  2 : show set of leds instant
  3 : show set of colors over whole strip, with interval
  4 : update one led, no reset
  5 : test
*/

// json conversion setup and params -----------------------------------------------------
uint8_t Ary[432];
uint8_t i = 0, j = 0;

// general setup and params --------------------------------------------------------------
bool crashed = false;
bool showUpdates = false;
bool beginning = true;
int NUM_PATTERNS = 3;
int currentPattern = 1;
bool pressed = true;

// Functions -----------------------------------------------------------
void PrintLn(String text)
{
  if (showUpdates)
  {
    Serial.println(text);
  }
}

void Print(String text)
{
  if (showUpdates)
  {
    Serial.print(text);
  }
}

// Setup ---------------------------------------------------------------
void setup() {
  Serial.begin(115200);  // Allows serial monitor output (check baud rate)
  delay(1500); // short delay for recovery
  pinMode(LED_PIN, OUTPUT); // Set LED_PIN as output
  digitalWrite(LED_PIN, LOW); // Set LED_PIN low to turn off build in LED
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // TODO Enable this if you want to use RGB LEDs ex: WS2812B
  // FastLED.addLeds<WS2812B, LED_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS)); // TODO Enable this if you want to use RGBW LEDs ex: SK6812

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  fillWhite();
  

  myButton.begin();  // initialize the button object

  Serial.println(" basic Setup done.\n");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    leds[0] = CRGB(10, 0, 0); // show blinking led when searching for wifi network
    FastLED.show();
    Print(".");
    delay(1000);
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
  }

  PrintLn("");
  PrintLn("Connected");

  leds[0] = CRGB(0, 50, 0); // show green led when connected to wifi
  FastLED.show();

  // do a first call to the website to get the led sequence
  setWebsite(1);
}


//---------------------------------------------------------------
void loop()
{
  if (beginning)
  {
    //fillBlack();
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
    beginning = false;  
  }
  //*
  readbutton();  // check for button press
  //*/
  if (pressed || currentPattern != 1)
  {
    PrintLn("Pressed function");
    PrintLn(String(currentPattern));
    if (currentPattern == 1)
    {
      PrintLn("White");
      fillWhite();   
    }
    else if (currentPattern == 2)
    {
      PrintLn("Calendar");
      Calendar();  
    }
    else if (currentPattern == 3)
    {
      runRed();      
    }
    pressed = false;
  }
}//end_main_loop


//---------------------------------------------------------------

void Calendar() {
  int ref = 60 / refreshRate;
  EVERY_N_SECONDS(ref) {
    PrintLn("Refreshrate");
    PrintLn(String(ref));
    if (!crashed) {
      setWebsite(1);
    }
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
    CallWebsite();
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
  }
}

void fillWhite() {
  int count = 0;
  for(int i = NUM_LEDS/2; i < NUM_LEDS; i++){
    leds[i] = CRGB(255,255,200);
    leds[i-(count*2)] = CRGB(255,255,200);
    //*
    if (i+2 < NUM_LEDS)
    {
      leds[i+2] = CRGB(8,8,8);
      leds[i-(count*2)-2] = CRGB(8,8,8);
    }
    if (i+4 < NUM_LEDS)
    {
      leds[i+4] = CRGB(2,2,2);
      leds[i-(count*2)-4] = CRGB(2,2,2);
    }
    if (i+6 < NUM_LEDS)
    {
      leds[i+6] = CRGB(1,1,1);
      leds[i-(count*2)-6] = CRGB(1,1,1);
    }
    //*/
    count = count + 1;
    FastLED.show();
    delay(15);
  }
  EVERY_N_MILLISECONDS(3000) {
    leds[0] = CRGB(200, 200, 50);
  }
}

void fillBlack(){
  int count = 0;
  for(int i = NUM_LEDS/2; i < NUM_LEDS; i++){
    leds[i] = CRGB(0, 0, 0);
    leds[i-(count*2)] = CRGB(0, 0, 0);
    count = count + 1;
    FastLED.show();
    delay(15);
  }
  delay(200);
}

void fillRed(){
  int count = 0;
  for(int i = NUM_LEDS/2; i < NUM_LEDS; i++){
    leds[i] = CRGB(10, 0, 0);
    leds[i-(count*2)] = CRGB(10, 0, 0);
    if (i+2 < NUM_LEDS)
    {
      leds[i+2] = CRGB(4, 0, 0);
      leds[i-(count*2)-2] = CRGB(4, 0, 0);
    }
    if (i+4 < NUM_LEDS)
    {
      leds[i+4] = CRGB(2, 0, 0);
      leds[i-(count*2)-4] = CRGB(2, 0, 0);
    }
    if (i+6 < NUM_LEDS)
    {
      leds[i+6] = CRGB(1, 0, 0);
      leds[i-(count*2)-6] = CRGB(1, 0, 0);
    }
    count = count + 1;
    FastLED.show();
    delay(30);
  }
  delay(200);
}
 void runRed()
 {
  EVERY_N_MILLISECONDS(3000) {
    PrintLn("Red");
    fillRed();
    fillBlack();
  }
 }
//---------------------------------------------------------------

//---------Function to read the button and do something----------
//   This section could be changed to use a potentiometer,
//   encoder, or something else to trigger a pattern change.
//   Depending on what was used, the nextPattern() function
//   might need to be updated as well to have things work
//   the way you intend.

void readbutton() {
  myButton.read();
  if (myButton.wasPressed()) {
    PrintLn("Button pressed!  Next pattern...   ");
    //nextPattern();  // Change to the next pattern
    pressed = true;
    currentPattern = (currentPattern + 1); 
    PrintLn(String(currentPattern));
    
    if (currentPattern > NUM_PATTERNS) {
      currentPattern = 1;
    }
    //Flash pixel zero white as a visual that button was pressed.
    leds[0] = CRGB(20, 20, 0); //Set first pixel color white
    FastLED.show();  //Update display
    delay(100);  //Short pause so we can see leds[0] flash on
    leds[0] = CRGB(0, 0, 0);  //Set first pixel off
    leds[1] = CRGB(0, 0, 0);  //leds[1] = CRGB::Black;  //Set second pixel off
    leds[2] = CRGB(0, 0, 0);  //Set third pixel off
    FastLED.show();
    delay(100);
  }
}//end_readbutton


//----------------------------------------------------------------------
void CallWebsite()
{
  // call the website to get an array of RGB values in return

  if (WiFi.status() == WL_CONNECTED)
  { //Check WiFi connection status
    HTTPClient http; //Declare an object of class HTTPClient
    String call = callWebsite;
    PrintLn(" ");
    PrintLn("___ FROM Here ___");
    PrintLn("call: " + String(call));
    http.begin(wifiClient, call);

    PrintLn("reaching site");
    int httpCode = http.GET(); //Send the request
    if (httpCode == 200)
    { //Check the returning code
      PrintLn("response: " + String(httpCode));
      String payload = http.getString(); //Get the request response payload
      PrintLn(payload);

      PrintLn("payload above"); //Print the response payload
      StringToJson(payload);
      crashed = false;
    }
    else
    {
      PrintLn("failed");
      crashed = true;
      //String payload = http.getString(); //Get the request response payload
      showRed();
      setWebsite(0);
      //PrintLn(payload);
    }
    http.end(); //Close connection
    PrintLn("___ TO Here ___");
    PrintLn(" ");
  }
}

void setWebsite(int type) {
  if (type == 1) {
    callWebsite = currWebsite + "?token=" + String(token);
  }
  else if (type == 2) {
    callWebsite = currWebsite + "?token=" + String(token) + "&refreshRate=6";
  }
  else if (type == 3) {
    callWebsite = String(website1) + "?token=" + String(token) + "&refreshRate=6" + "&mode=1" + "&action=Calendar" + "&website=" + String(website1);
  }
  else  {
    callWebsite = currWebsite + "?token=" + String(token) + "&refreshRate=6";
  }
}

void StringToJson(String textIn)
{
  // convert the string into a json document.
  DynamicJsonDocument doc(14999);
  deserializeJson(doc, textIn);
  JsonObject obj = doc.as<JsonObject>();
  JsonToMode(obj);
  JsonToFastled(obj);
  JsonToRefreshRate(obj);
  JsonToWebsite(obj);
  JsonToAction(obj);
}

int arraySize(JsonObject obj) {
  JsonArray array = obj[String("LedSequence")];
  int arraysize = array.size();
  return arraysize;
}

void JsonToFastled(JsonObject obj)
{
  // put the values of the json document into the ledstring
  if (useMode == 1)
  { // slow showing of set
    PrintLn("Mode 1");
    //int maxi = arraySize(obj)+1;
    for (int i = 0; i < NUM_LEDS; i++)
    {
      int R = obj[String("LedSequence")][i][0];
      int G = obj[String("LedSequence")][i][1];
      int B = obj[String("LedSequence")][i][2];
      leds[i] = CRGB(R, G, B);
      leds[0] = CRGB(1, 0, 0);
      FastLED.show();
      delay(30);
    }
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
    delay(30);
  }
  else if (useMode == 2)
  { // show set instant
    PrintLn("Mode 2");
    for (int i = 0; i < NUM_LEDS; i++)
    {
      int R = obj[String("LedSequence")][i][0];
      int G = obj[String("LedSequence")][i][1];
      int B = obj[String("LedSequence")][i][2];
      leds[i] = CRGB(R, G, B);
    }
    FastLED.show();
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
    delay(30);
  }
  else if (useMode == 3)
  { // show set of colors over whole strip, with interval
    PrintLn("Mode 3");
    fullBar(obj, true);
  }
  else if (useMode == 4)
  { // show one led
    PrintLn("Mode 4");
    oneLed(obj);
  }
  else if (useMode == 5)
  { // slow showing of set + dimming
    PrintLn("Mode 5");
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(0, 0, 0);
      FastLED.show();
      delay(30);
      int R = obj[String("LedSequence")][i][0];
      int G = obj[String("LedSequence")][i][1];
      int B = obj[String("LedSequence")][i][2];
      leds[i] = CRGB(R, G, B);
      FastLED.show();
      delay(30);
    }
  }
  else if (useMode == 6)
  { // fast flash
    PrintLn("Mode 6");
    // save current
    //prevleds = leds; // save previous
    for (int i = 0; i < 144; i++)
    {
      prevleds[i] = leds[i];
    }
    fullBar(obj, false);
    for (int i = 0; i < 144; i++) // take back te previous
    {
      leds[i] = prevleds[i];
    }
    FastLED.show();
  }
  else
  {
    PrintLn("Mode Else");
    //*
    leds[0] = CRGB(1, 0, 0); // something is wrong
    FastLED.show();
    delay(300);
    leds[0] = CRGB(0, 0, 0);
    FastLED.show();
    delay(300);
    //*/
  }
}

void fullBar(JsonObject obj, bool slow)
{
  int interval = obj[String("LedSequence")][0][0]; // first array contains the interval
  int maxi = arraySize(obj);
  for (int i = 1; i < maxi; i++)
  {
    int R = obj[String("LedSequence")][i][0];
    int G = obj[String("LedSequence")][i][1];
    int B = obj[String("LedSequence")][i][2];
   //0 fill_solid(leds, NUM_LEDS, CRGB(R, G, B));
    FastLED.show();
    if (slow)
    {
      if (i < maxi - 1)
      {
        delay(interval);
      }
    }
    else
    {
      delay(interval);
    }
  }
}

void oneLed(JsonObject obj)
{
  int pos = obj[String("LedSequence")][0][0]; // first array contains the led position
  int R = obj[String("LedSequence")][1][0];
  int G = obj[String("LedSequence")][1][1];
  int B = obj[String("LedSequence")][1][2];
  leds[pos] = CRGB(R, G, B);
  FastLED.show();
}

void JsonToRefreshRate(JsonObject obj)
{

  if (obj[String("refreshRate")] != 0) {
    refreshRate = obj[String("refreshRate")];
  }
  Print("refreshrate: ");
  PrintLn(String(refreshRate));
}

void JsonToWebsite(JsonObject obj)
{
  currWebsite = obj[String("website")].as<String>();
  Print("website: ");
  PrintLn(currWebsite);
}

void JsonToMode(JsonObject obj)
{
  useMode = obj[String("mode")];
  Print("mode: ");
  PrintLn(String(useMode));
}

void JsonToAction(JsonObject obj)
{
  useAction = obj[String("action")].as<String>();
  Print("action: ");
  PrintLn(String(useAction));
}

void showRed() {
  /*
  leds[0] = CRGB(20, 0, 0); // show blinking led when searching for wifi network
  FastLED.show();
  delay(1000);
  leds[0] = CRGB(1, 0, 0); // show blinking led when searching for wifi network
  FastLED.show();
  */
}
