/*  weather shield.
 *
 *      Uses the DarkSky API to display the temperature and the 7 day
 *  forecast on the shield. The clever bit is how we parse a JSON response
 *  that is bigger than will fit in memory.
 */

#include <Ethernet2.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "DarkSkyParser.h"
#include "wfont.h"
#include "largenum.h"
#include "DateTime.h"

#define INTERVAL    3600000      /* Every hour */

/*
 *  Display interface setup
 */

// We've modified the wiring to use TFT_CS == 8 instead of 10...
#define TFT_DC 9
#define TFT_CS 8

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

/*
 *  Ethernet setup
 */

             // Ethernet MAC code from bottom of unit
byte mac[] = { 0x2C, 0xF7, 0xF1, 0x08, 0x1E, 0xE1 };

// Replace with server and DarkSky API Key to make this work.
// Update if using the WiFi shield.
#define SERVER  "(server).com"
#define URI     "/wforward/api/(DarkSkyAppKey)/35.946,-78.61"

EthernetClient client;     // The Ethernet Client
DarkSkyParser parser;      // Dark sky parser
unsigned long nextTime;

/*
 *  Setup
 */

void setup() 
{
    /*
     *  Start up our system. We print debugging information as we start
     *  our program for the first time.
     */
    tft.begin();
    tft.setRotation(1);

    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.println(F("Starting up Ethernet connection."));

    if (Ethernet.begin(mac) == 0) {
        tft.println(F("Ethernet startup error"));
        for (;;) ;  // Halt.
    }

    tft.println(F("Ethernet running."));
    delay(1000);        // Delay 1 second

    nextTime = 0;
}

void loop()
{
    tft.setFont(NULL);

    long t = millis();
    if (nextTime > t) return;  /* Skip */
    nextTime = t + INTERVAL;
    
    /*
     *  Send request
     */

    tft.println(F("Connecting to " SERVER));
    if (!client.connect(SERVER,80)) {
        tft.setCursor(0,232);
        tft.println(F("Unable to connect to server"));
        return;
    }

    client.println("GET " URI " HTTP/1.1");
    client.println("Host: " SERVER);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    parser.reset();

    tft.setCursor(0,232);
    tft.println(F("Loading weather information..."));
    for (;;) {
        if (client.available()) {
            char c = client.read();
            parser.addCharacter(c);
        }
        if (!client.connected()) {
            client.stop();
            break;
        }
    }

    /*
     *  We have results. Show them
     */

    tft.fillScreen(ILI9341_BLACK);

    tft.setFont(NULL);
    tft.setTextColor(ILI9341_LIGHTGREY);
    for (int8_t i = 0; i < 7; ++i) {
        tft.setCursor(45*i+20,125);
        switch (parser.forecast[i].dow) {
            case 0: tft.print(F("SUN"));    break;
            case 1: tft.print(F("MON"));    break;
            case 2: tft.print(F("TUE"));    break;
            case 3: tft.print(F("WED"));    break;
            case 4: tft.print(F("THU"));    break;
            case 5: tft.print(F("FRI"));    break;
            case 6: tft.print(F("SAT"));    break;
        }

        tft.setCursor(45*i+15,160);
        uint8_t p = parser.forecast[i].precip;
        if (p < 100) tft.print(' ');
        if (p < 10) tft.print(' ');
        tft.print(p);
        tft.print(F("%"));
    }

    /*
     * Date/Time
     */

    DateTimeRecord dtr = UnixToDate(parser.lastTime + parser.offset * 60);
    tft.setCursor(180,232);
    tft.print(dtr.month);
    tft.print(F("/"));
    tft.print(dtr.day);
    tft.print(F("/"));
    tft.print(dtr.year);
    tft.print(F(" @ "));
    tft.print(dtr.hr);
    tft.print(F(":"));
    if (dtr.min < 10) tft.print('0');
    tft.print(dtr.min);

    /*
     *  Fine print first
     */
    tft.setCursor(150,18);
    tft.print(parser.precip);
    tft.print(F("%"));

    tft.setCursor(120,40);
    tft.print(F("Wind: "));
    tft.print(parser.windSpeed);
    tft.print(" at ");
    tft.print(parser.windDirection);
    tft.setCursor(220,40);
    tft.print("Pressure: ");
    tft.print(parser.pressure);

    /*
     *  Temperature information
     */
    tft.setTextColor(ILI9341_WHITE);
    tft.setFont(&largenum);
    tft.setCursor(10,60);
    tft.print(parser.temperature);

    tft.setFont(&wfont);
    tft.setCursor(120,30);
    tft.print((char)('A' + parser.icon - 1));

    for (int8_t i = 0; i < 7; ++i) {
        tft.setCursor(45*i+20,155);
        tft.setTextColor(ILI9341_WHITE);
        tft.print((char)('A' + parser.forecast[i].icon - 1));
        tft.setCursor(45*i+15,190);
        tft.setTextColor(ILI9341_RED);
        tft.print(parser.forecast[i].high);
        tft.setCursor(45*i+15,215);
        tft.setTextColor(ILI9341_BLUE);
        tft.print(parser.forecast[i].low);
    }
}

