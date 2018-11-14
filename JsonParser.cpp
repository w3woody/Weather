//
//  JsonParser.cpp
//  TestParser
//
//  Created by William Woody on 11/13/18.
//  Copyright © 2018 Glenview Software. All rights reserved.
//

#include <stdlib.h>
#include "JsonParser.h"

#define STATE_NONE                0       /* Parsing any value */
#define STATE_INSTRING          1       /* String */
#define STATE_HASSTRING         2       /* String */
#define STATE_INNUMBER          3       /* String */
#define STATE_HASNUMBER         4       /* String */
#define STATE_INBOOL            5
#define STATE_HASBOOL           6
#define STATE_INNULL            7
#define STATE_HASNULL           8

JsonParser::JsonParser()
{
    reset();
}

JsonParser::~JsonParser()
{
}

/*
 *  Hooks
 */

void JsonParser::startDocument()
{
}

void JsonParser::endDocument()
{
}

void JsonParser::stringValue(char *str)
{
}

void JsonParser::doubleValue(double value)
{
}

void JsonParser::booleanValue(bool value)
{
}

void JsonParser::nullValue()
{
}

void JsonParser::startArray()
{
}

void JsonParser::startObject()
{
}

void JsonParser::objectKey(char *str)
{
}

void JsonParser::endObject()
{
}

void JsonParser::endArray()
{
}

void JsonParser::error()
{
}

/*
 *  Entry points
 */

void JsonParser::reset()
{
    bpos = 0;
    state = STATE_NONE;
    err = false;
}

void JsonParser::start()
{
    startDocument();
}

void JsonParser::flushValue()
{
    if (state == STATE_HASSTRING) {
        buffer[bpos] = 0;
        stringValue(buffer);
    } else if (state == STATE_HASNUMBER) {
        buffer[bpos] = 0;
        doubleValue(atof(buffer));
    } else if (state == STATE_HASBOOL) {
        booleanValue(buffer[0] == 't');
    } else if (state == STATE_HASNULL) {
        nullValue();
    }
    state = STATE_NONE;
}

void JsonParser::end()
{
    flushValue();
    endDocument();
}

/*
 *  Internal status
 */

void JsonParser::doError()
{
    if (!err) {
        err = true;
        error();
    }
}

void JsonParser::addStringChar(char ch)
{
    if (bpos >= JSONSTRINGLEN-1) return;
    buffer[bpos++] = ch;
}

/*
 *  Handle character parsing
 */

void JsonParser::addCharacter(char ch)
{
    if (err) return;

    /*
     *  Take appropriate action depending on what we're parsing. Basically
     *  we are either parsing a string, a number, a constant or parsing
     *  syntax.
     */

    if (state == STATE_INSTRING) {
        if (inEscape) {
            addStringChar(ch);
            inEscape = false;
        } else if (ch == '"') {
            state = STATE_HASSTRING;
        } else if (ch == '\\') {
            inEscape = false;
        } else {
            addStringChar(ch);
        }
        return;

    } else if (state == STATE_INNUMBER) {
        if ((ch == '-') || ((ch >= '0') && (ch <= '9')) || (ch == '.')) {
            addStringChar(ch);
            return;
        } else {
            state = STATE_HASNUMBER;
        }

    } else if (state == STATE_INBOOL) {
        if ((ch >= 'a') && (ch <= 'z')) return;
        else state = STATE_HASBOOL;

    } else if (state == STATE_INNULL) {
        if ((ch >= 'a') && (ch <= 'z')) return;
        else state = STATE_HASNULL;

    }

    if (ch == ':') {
        if (state == STATE_HASSTRING) {
            buffer[bpos] = 0;
            objectKey(buffer);
            state = STATE_NONE;
        }
    } else if (ch == '{') {
        flushValue();
        startObject();
    } else if (ch == '[') {
        flushValue();
        startArray();
    } else if (ch == '}') {
        flushValue();
        endObject();
    } else if (ch == ']') {
        flushValue();
        endArray();
    } else if (ch == ',') {
        flushValue();
    } else if ((ch == 't') || (ch == 'f')) {
        flushValue();
        bpos = 0;
        addStringChar(ch);
        state = STATE_INBOOL;
    } else if (ch == 'n') {
        flushValue();
        bpos = 0;
        addStringChar(ch);
        state = STATE_INNULL;
    } else if (ch == '"') {
        flushValue();
        bpos = 0;
        inEscape = false;
        state = STATE_INSTRING;
    } else if ((ch == '-') || ((ch >= '0') && (ch <= '9'))) {
        flushValue();
        bpos = 0;
        state = STATE_INNUMBER;
        addStringChar(ch);
    }
}


