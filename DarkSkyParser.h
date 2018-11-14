//
//  DarkSkyParser.h
//  TestParser
//
//  Created by William Woody on 11/13/18.
//  Copyright © 2018 Glenview Software. All rights reserved.
//

#ifndef DarkSkyParser_h
#define DarkSkyParser_h

#include <stdint.h>
#include "JsonParser.h"

#define ICON_NULL                    0
#define ICON_CLEAR_DAY              1
#define ICON_CLEAR_NIGHT            2
#define ICON_RAIN                   3
#define ICON_SNOW                   4
#define ICON_SLEET                  5
#define ICON_WIND                   6
#define ICON_FOG                    7
#define ICON_CLOUDY                 8
#define ICON_PARTLY_CLOUDY_DAY      9
#define ICON_PARTLY_CLOUDY_NIGHT    10

struct Day {
    uint8_t dow;
    int16_t high;
    int16_t low;
    uint8_t precip;     // 0 - 100
    uint8_t icon;       // Icon constant
};

class DarkSkyParser : public JsonParser
{
    public:
                DarkSkyParser();
                ~DarkSkyParser();

        virtual void startDocument();
        virtual void endDocument();

        virtual void stringValue(char *str);
        virtual void doubleValue(double value);
        virtual void booleanValue(bool value);
        virtual void nullValue();

        virtual void startArray();
        virtual void endArray();

        virtual void startObject();
        virtual void objectKey(char *str);
        virtual void endObject();

        virtual void error();

        /*
         *  Current status
         */

        int16_t temperature;
        uint8_t icon;
        uint8_t windSpeed;
        uint8_t precip;
        uint16_t windDirection;
        uint16_t pressure;
        uint32_t lastTime;

        Day forecast[7];

        int16_t offset;

    private:
        bool currently;
        bool daily;
        bool inArray;
        bool offsetFlag;
        int8_t state;
        int8_t dcount;

        Day day;
};

#endif /* DarkSkyParser_h */

