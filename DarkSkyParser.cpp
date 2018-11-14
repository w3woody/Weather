//
//  DarkSkyParser.cpp
//  TestParser
//
//  Created by William Woody on 11/13/18.
//  Copyright © 2018 Glenview Software. All rights reserved.
//

#include <string.h>
#include "DarkSkyParser.h"

#define CURRENT_TEMPERATURE     1
#define CURRENT_PRESSURE        2
#define CURRENT_WINDSPEED       3
#define CURRENT_BEARING         4
#define CURRENT_ICON            5
#define CURRENT_PRECIP          6
#define CURRENT_TIME            7

#define DAY_DOW                 10
#define DAY_HIGH                11
#define DAY_LOW                 12
#define DAY_PRECIP              13
#define DAY_ICON                14


DarkSkyParser::DarkSkyParser()
{
}

DarkSkyParser::~DarkSkyParser()
{
}

void DarkSkyParser::startDocument()
{
}

void DarkSkyParser::endDocument()
{
}

static int iconToIndex(const char *str)
{
    if (!strcmp(str,"clear-day")) return ICON_CLEAR_DAY;
    else if (!strcmp(str,"clear-night")) return ICON_CLEAR_NIGHT;
    else if (!strcmp(str,"rain")) return ICON_RAIN;
    else if (!strcmp(str,"snow")) return ICON_SNOW;
    else if (!strcmp(str,"sleet")) return ICON_SLEET;
    else if (!strcmp(str,"wind")) return ICON_WIND;
    else if (!strcmp(str,"fog")) return ICON_FOG;
    else if (!strcmp(str,"cloudy")) return ICON_CLOUDY;
    else if (!strcmp(str,"partly-cloudy-day")) return ICON_PARTLY_CLOUDY_DAY;
    else if (!strcmp(str,"partly-cloudy-night")) return ICON_PARTLY_CLOUDY_NIGHT;
    else return ICON_NULL;
}

void DarkSkyParser::stringValue(char *str)
{
    if (state == DAY_ICON) {
        day.icon = iconToIndex(str);
    } else if (state == CURRENT_ICON) {
        icon = iconToIndex(str);
    }
}

void DarkSkyParser::doubleValue(double value)
{
    if (offsetFlag) {
        offset = (int16_t)(60 * value);
        offsetFlag = false;
    } else if (state == CURRENT_TEMPERATURE) {
        temperature = (int16_t)value;
    } else if (state == CURRENT_PRESSURE) {
        pressure = (uint16_t)value;
    } else if (state == CURRENT_WINDSPEED) {
        windSpeed = (uint8_t)value;
    } else if (state == CURRENT_BEARING) {
        windDirection = (uint16_t)value;
    } else if (state == CURRENT_PRECIP) {
        precip = (uint8_t)(100 * value);
    } else if (state == CURRENT_TIME) {
        lastTime = (uint32_t)value;
    } else if (state == DAY_DOW) {
        day.dow = (uint8_t)(((int32_t)(4 + value / 86400)) % 7);
    } else if (state == DAY_HIGH) {
        day.high = (int16_t)value;
    } else if (state == DAY_LOW) {
        day.low = (int16_t)value;
    } else if (state == DAY_PRECIP) {
        day.precip = (uint8_t)(100 * value);
    }

    state = 0;
}

void DarkSkyParser::booleanValue(bool value)
{
}

void DarkSkyParser::nullValue()
{
}

void DarkSkyParser::startArray()
{
    if (daily) {
        inArray = true;
    }
}

void DarkSkyParser::endArray()
{
    if (daily) {
        inArray = false;
    }
}

void DarkSkyParser::startObject()
{
}

void DarkSkyParser::objectKey(char *str)
{
    if (!strcmp(str,"currently")) {
        currently = true;
        daily = false;
    } else if (!strcmp(str,"daily")) {
        daily = true;
        currently = false;
        dcount = 0;
    } else if (!strcmp(str,"hourly") || !strcmp(str,"minutely")) {
        daily = false;
        dcount = 0;
        currently = false;
    } else if (!strcmp(str,"offset")) {
        offsetFlag = true;
    } else if (daily) {
        if (!strcmp(str,"temperatureMax")) {
            state = DAY_HIGH;
        } else if (!strcmp(str,"temperatureMin")) {
            state = DAY_LOW;
        } else if (!strcmp(str,"precipProbability")) {
            state = DAY_PRECIP;
        } else if (!strcmp(str,"icon")) {
            state = DAY_ICON;
        } else if (!strcmp(str,"time")) {
            state = DAY_DOW;
        }
    } else if (currently) {
        if (!strcmp(str,"temperature")) {
            state = CURRENT_TEMPERATURE;
        } else if (!strcmp(str,"pressure")) {
            state = CURRENT_PRESSURE;
        } else if (!strcmp(str,"windSpeed")) {
            state = CURRENT_WINDSPEED;
        } else if (!strcmp(str,"windBearing")) {
            state = CURRENT_BEARING;
        } else if (!strcmp(str,"icon")) {
            state = CURRENT_ICON;
        } else if (!strcmp(str,"time")) {
            state = CURRENT_TIME;
        } else if (!strcmp(str,"precipProbability")) {
            state = CURRENT_PRECIP;
        }
    }
}

void DarkSkyParser::endObject()
{
    if (inArray) {
        // in daily array, just loaded object.
        if (dcount < 7) {
            forecast[dcount++] = day;
        }
    }
}

void DarkSkyParser::error()
{
}


