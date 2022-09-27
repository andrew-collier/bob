#ifndef DATA_H
#define DATA_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_UNITS 490
#define SIZEOF_BUFFER 5100
#define MAX_CHAR 50
#define FIELDS 30

char**** get_dataArray(void);
void free_memory(char**** data);
void mem_error(void);
void extract_data_from_source(const char* argv[], char**** data, size_t* tc, size_t* tu);
extern size_t arg_c;

/* Array Legend */
// 0	unit number
// 1	tenant id
// 2	first name
// 3	last name
// 4	company
// 5	addr1
// 6	addr2
// 7	city
// 8	state
// 9	zip
// 10	email
// 11	unit width
// 12	unit length
// 13	ledger ID
// 14	rent
// 15	Scheduled rent
// 16	Scheduled rent date
// 17	Rent balance
// 18	Late fee balance
// 19	NSF fee balance
// 20	Lock cut fee balance
// 21	Auction fee balance
// 22	Other fee balance (certified letter)
// 23	Paid thru date
// 24	prepay credit
// 25	period duration (months)
// 26	advance delivery
// 27	period from
// 28	period to
// 29	period late

#endif
