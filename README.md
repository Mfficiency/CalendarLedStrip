# CalendarLeds
<img src="./explanation_card.png">

Get your google calendar into an individual addressable LED strip.

## Intro

It reads your google calendar and returns for each led an RGB value.

144 Ledstrip example
led #1 = 00:10 - 00:19 > this is so you can have a bright led at the end of the ledstrip indicating "the end of the day"
led #2 = 00:20 - 00:29
...
led #144 = 00:00 - 00:09 

### Items used:
- ESP8266 node mcu V3
- WS2812B, 144leds, 1m
- Aluminum V profile with diffuser
- Wires
- Tape
- Hot glue

### Resources:
- [Calendar api tutorial](https://www.youtube.com/watch?v=zrLf4KMs71E)
- [Fastled RGBW hack](https://www.partsnotincluded.com/fastled-rgbw-neopixels-sk6812/) 
- [Button library](https://github.com/JChristensen/JC_Button)

## Getting started
1. Follow the calendar api tutorial.
2. Copy my code into your google cloud project.
3. Modify example files to your needs.
4. Modify TODO's in the .ino file
5. Push to a microcontroller.
6. It should work 🤷‍♂️

### Common issues & fixes:
- first led is flickering
	- cover first led with tape 👌
- rest of the strip is sometimes ghosting/flickering
	- check wiring 🧐
- wintertime/summertime is hardcoded
	- change the code 🙃
- microcontroller is not stable & resets the website url so no leds are showing
	- change the code 😬 OR have a shortcut in your browser to reactivate the api