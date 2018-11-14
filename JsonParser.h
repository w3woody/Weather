//
//  JsonParser.h
//  TestParser
//
//  Created by William Woody on 11/13/18.
//  Copyright © 2018 Glenview Software. All rights reserved.
//

#ifndef JsonParser_h
#define JsonParser_h

#include <stdio.h>

#define JSONSTRINGLEN            128
#define JSONSTACKLEN            16

/*  JsonParser
 *
 *      A simple state-driven parser with callbacks
 */

class JsonParser
{
    public:
                JsonParser();
                ~JsonParser();

        void    reset();

        void    start();
        void    addCharacter(char ch);
        void    end();

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

    private:
        void doError();
        void addStringChar(char ch);
        void flushValue();

        bool err;
        bool inEscape;
        bool hasValue;

        int state;

        int bpos;
        char buffer[JSONSTRINGLEN];
};

#endif /* JsonParser_h */

