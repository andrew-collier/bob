#include <ctype.h>

#include "groff.h"
#include <time.h>

struct Date date;
struct tm *today;
struct tm *from;
struct tm *to;
struct tm *late;
struct tm *schedIncrease;


void process_data(char****, size_t, size_t*);
void remove_spaces(char*);

void init_time(void)
{
}


void mkroff(char**** data, size_t* tc, size_t* tu)
{
	for (size_t tenant=0; tenant<*tc; tenant++) {
		process_data(data, tenant, tu);
	}
	return;
}


void process_data(char**** data, size_t tenant, size_t* tu)
{
	/* initialize today's date */
	date.today = time(NULL);
	today = localtime(&date.today); // struct tm *today
	strftime(date.Today, 100, DATE_FORMAT, today);

	/* initialize due date */
	date.from = time(NULL);
	from = localtime(&date.from);
	from->tm_mday = 1;

	/* initialize due date */
	date.to = time(NULL);
	to = localtime(&date.to);
	to->tm_mday = 1;

	/* initialize due date */
	date.schedIncrease = time(NULL);
	schedIncrease = localtime(&date.schedIncrease);

	/* Generate Addressee Info */
	char addressee[MAX_CHAR];
	char* addresseePtr = addressee;
	size_t filenameNameIndex;
	bool companyUnit = false;

	if (strcmp(data[tenant][0][4], "") != 0) {
		filenameNameIndex = 4;
		companyUnit = true;
	}
	else {
		filenameNameIndex = 3;
	}

	/* Generate filename */
	char filenamevar[100];
	char *filename = filenamevar;
	sprintf(addressee, "%s", data[tenant][0][filenameNameIndex]);
	remove_spaces(addresseePtr);
	if (tu[tenant] == 1)
		sprintf(filenamevar, "%s_%s_%s_invoice.groff", data[tenant][0][0], data[tenant][0][1], addresseePtr);
	else
		sprintf(filenamevar, "%s_%s_invoice.groff", data[tenant][0][1], addresseePtr);

	/* Directory path */
	char outputPath[200];
	/* Output groff.mom to $HOME/.local/share/bob/groff */
	sprintf(outputPath, "%s%s%s", getenv("XDG_DATA_HOME"), "/bob/groff/", filename);

	/* Start writing file */
	FILE *fp = fopen(outputPath, "w");
	if (fp == NULL) {
		printf("Error: Unable to open file for writing.\n");
		exit(1);
	}

	/* document setup */
	fprintf(fp, "%s\t%s\n", ".AUTHOR", "\"Bronson Mini Storage\"");
	fprintf(fp, "%s\t%s\n", ".DOCTYPE", "LETTER");
	fprintf(fp, "%s\t%s\n", ".PRINTSTYLE", "TYPESET");
	fprintf(fp, "%s\t%s\n", ".T_MARGIN", "2c");
	fprintf(fp, "%s\t%s\n", ".R_MARGIN", "2c");
	fprintf(fp, "%s\t%s\n", ".L_MARGIN", "2c");
	fprintf(fp, "%s\t%s\n", ".B_MARGIN", "2c");
	fprintf(fp, "%s\n", ".START");

	/* From Bronson Mini Storage */
	fprintf(fp, "%s\n", ".FROM");
	fprintf(fp, "%s\n%s\n%s\n", "Bronson Mini Storage", "16120 NW Bronson Rd", "Beaverton, OR 97006");
	
	/* Date */
	fprintf(fp, "%s\n", ".SPACE");
	fprintf(fp, "%s\n", ".DATE");
	fprintf(fp, "%s\n", date.Today);

	/* To Addressee */
	fprintf(fp, "%s\n", ".TO");
	if ( companyUnit == true )
		fprintf(fp, "%s\nC/O " , data[tenant][0][4]); // Company name
	fprintf(fp, "%s %s\n", data[tenant][0][2], data[tenant][0][3]); // First/Last
	fprintf(fp, "%s\n", data[tenant][0][5]); // Addr1
	if (strcmp(data[tenant][0][6], "") !=0 )
		fprintf(fp, "%s\n", data[tenant][0][6]); // Addr2
	fprintf(fp, "%s, %s %s\n", data[tenant][0][7],data[tenant][0][8],data[tenant][0][9]); // City, ST zip
	fprintf(fp, "%s\n", ".SPACE");

	/* Greeting */
	fprintf(fp, "%s\n", ".GREETING");
	fprintf(fp, "%s %s %s,\n", "Dear", data[tenant][0][2], data[tenant][0][3]);
	fprintf(fp, "%s\n", ".PP");
	fprintf(fp, "%s\n", ".HY OFF");
	fprintf(fp, "%s\n", ".IQ CLEAR");
	if (tu[tenant] > 1)
		fprintf(fp, "%s\n", "Below are the upcoming charges for your units here at Bronson Mini Storage.");
	else
		fprintf(fp, "%s\n", "Below are the upcoming charges for your unit here at Bronson Mini Storage.");
	/* Table */
	fprintf(fp, "%s\n", ".TS");
	fprintf(fp, "%s\n", "box ;");
	fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s.\n", "c", "c", "c", "c", "c", "c", "c", "c", "c");
	fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n", "Unit", "Size", "Rate", "From", "To", "Rent", "Discount", "Prior Bal.", "Charge");

	float rentCharge, discount, balance, grandTotal;
	grandTotal = balance = 0;
	/* Per unit */
	for (size_t i=0; i<tu[tenant]; i++) { // For each unit tenant has...
		fprintf(fp, "%s\t", data[tenant][i][0]); // Unit
		fprintf(fp, "%gx%g\t", atof(data[tenant][i][11]), atof(data[tenant][i][12])); // Size

		/* scheduled rent determination */
		strptime(data[tenant][i][16], "%m/%d/%Y %I:%M:%S %p", schedIncrease);
		schedIncrease->tm_mon += 1;
		schedIncrease->tm_year = schedIncrease->tm_year + 1900;

		if ( mktime(schedIncrease) == date.from )
			strcpy(data[tenant][i][14], data[tenant][i][15]);

		/* rent */
		fprintf(fp, "$%.2f\t", atof(data[tenant][i][14])); // rent

		/* from */
		strptime(data[tenant][i][23], "%m/%d/%Y %I:%M:%S %p", from);
		if (i==0)
			from->tm_mday += 1;
		date.from = mktime(from);
		from = localtime(&date.from);
		strftime(date.From,100,DATE_PRINT_FORMAT,from);
		fprintf(fp, "%s\t", date.From);

		/* to */
		date.to = mktime(from);
		to = localtime(&date.from);
		if (i==0)
			to->tm_mday -= 1;
		to->tm_mon += atoi(data[tenant][i][25]);
		date.to = mktime(to);
		to = localtime(&date.to);
		strftime(date.To,100,DATE_PRINT_FORMAT,to);
		fprintf(fp, "%s\t", date.To);

		/* rent charge */
		rentCharge = atof(data[tenant][i][25]) * atof(data[tenant][i][14]);
		fprintf(fp, "$%.2f\t", rentCharge);

		/* discount */
		switch (atoi(data[tenant][i][25])) {
			case 3:
			case 6:
			case 12:
				discount = 0.05 * atof(data[tenant][i][25]) * atof(data[tenant][i][14]);
				break;
			default:
				discount = 0.00;
		}
		fprintf(fp, "($%.2f)\t", discount);

		/* Balance */
		for (int charge=17; charge<=22; charge++)
			balance += atof(data[tenant][i][17]);
		balance -= atof(data[tenant][i][24]);

		if (balance < 0) {
			fprintf(fp, "($%.2f)\t", balance*-1);
		}
		else
			fprintf(fp, "%.2f\t", balance);
		
		/* per-unit total */
		fprintf(fp, "$%.2f\t\n", rentCharge-discount+balance);

		/* sub-total */
		grandTotal += rentCharge-discount+balance;
	}



	fprintf(fp, "%s\n", ".TE");
	fprintf(fp, "\t\t\t\t\t\t%s %s: $%.2f\n", "Total due", date.From, grandTotal);

	fprintf(fp, "%s\n", ".PP");
	fprintf(fp, "%s\n", ".HY OFF");
	fprintf(fp, "%s\n", ".IQ CLEAR");
	fprintf(fp, "%s\n", "Please make checks payable to \"Bronson Mini Storage\" and remit to the return address above. Otherwise, payment can be made using a debit or credit card online at https://bronsonministorage.com, or by calling the office at (503) 614-0632. Please contact us if you have any questions.");




	fprintf(fp, "%s\n%s\n", ".CLOSING", "Thank you,");


	fclose(fp);
}

void remove_spaces(char* s)
{
	int i=0;
	while (s[i]) {
		if (isspace(s[i]))
			s[i] = '_';
		if (s[i] == '.')
			memmove(&s[i], &s[i+1], strlen(s)-i);
		i++;
	}
}
