//
//  DateTime.h
//  TestParser
//
//  Created by William Woody on 11/13/18.
//  Copyright © 2018 Glenview Software. All rights reserved.
//

#ifndef DateTime_h
#define DateTime_h

#include <stdint.h>

struct DateTimeRecord
{
	uint8_t sec;
	uint8_t min;
	uint8_t hr;
	uint8_t day;
	uint8_t month;
	uint8_t dow;
	uint16_t year;
};

extern DateTimeRecord UnixToDate(uint32_t time);

#endif /* DateTime_hpp */
