//
//  DateTime.cpp
//  TestParser
//
//  Created by William Woody on 11/13/18.
//  Copyright © 2018 Glenview Software. All rights reserved.
//

#include "DateTime.h"

#define EPOCH		719163

/*	IsLeapYear
 *
 *		Is this a leap year?
 */

static bool IsLeapYear(uint16_t year)
{
	if (year % 4) return false;			/* like 2017 */
	if (year % 100) return true;		/* like 1996 */
	if (year % 400) return false;		/* like 1900 */
	return true;						/* like 2000 */
}

static uint32_t GregorianDayCount(uint8_t day, uint8_t month, uint16_t year)
{
	uint16_t y1 = year - 1;
	uint32_t tmp;

	tmp = 365 * ((uint32_t)y1);
	tmp += y1/4;
	tmp -= y1/100;
	tmp += y1/400;
	tmp += (367 * (uint16_t)month - 362)/12;

	if (month > 2) {
		tmp -= IsLeapYear(year) ? 1 : 2;
	}

	return tmp + day - EPOCH;
}

static uint16_t GregorianYear(uint32_t dcount)
{
	uint32_t d;

	d = dcount + EPOCH - 1;
	uint8_t  n400 = d / 146097;

	d %= 146097;
	uint8_t  n100 = d / 36524;

	d %= 36524;
	uint8_t  n4   = d / 1461;

	d %= 1461;
	uint8_t  n1   = d / 365;

	int16_t year = 400 * (uint16_t)n400 + 100 * (uint16_t)n100 + 4 * n4 + n1;
	if ((n100 == 4) || (n1 == 4)) return year;
	return year + 1;
}

void GregorianDate(uint32_t dcount, DateTimeRecord &ret)
{
	ret.year = GregorianYear(dcount);
	uint16_t priorDays = dcount - GregorianDayCount(1,1,ret.year);
	uint8_t correction;
	uint32_t march = GregorianDayCount(1,3,ret.year);	// march 1

	if (dcount < march) {
		correction = 0;
	} else if (IsLeapYear(ret.year)) {
		correction = 1;
	} else {
		correction = 2;
	}

	ret.month = (12 * (priorDays + correction) + 373)/367;
	ret.day = dcount - GregorianDayCount(1,ret.month,ret.year) + 1;
}

DateTimeRecord UnixToDate(uint32_t time)
{
	DateTimeRecord dr;

	uint32_t scount = time % 86400;		// Second element
	uint32_t dcount = time / 86400;

	GregorianDate(dcount,dr);
	dr.dow = (dcount+4) % 7;			// Sunday == 0, 1/1/1970 is Thursday (4)
	dr.sec = scount % 60;
	scount /= 60;
	dr.min = scount % 60;
	scount /= 60;
	dr.hr = scount;

	return dr;
}
