#ifndef GROFF_H
#define GROFF_H

#define __USE_XOPEN
#include <time.h>
#include "data.h"

#define DATE_FORMAT "%m/%d/%Y"
//#define DATE_PRINT_FORMAT "%b %d, %Y"
#define DATE_PRINT_FORMAT "%m/%d/%y"

void mkroff(char**** data, size_t* tc, size_t* tu);

struct Date {
	time_t today;
	time_t from;
	time_t to;
	time_t late;
	time_t schedIncrease;
	char Today[20];
	char From[20];
	char To[20];
	char Late[20];
	char SchedIncrease[20];
};

extern struct Date date;
extern struct tm *today;
extern struct tm *from;
extern struct tm *to;
extern struct tm *late;
extern struct tm *schedIncrease;

#endif
