#include "data.h"

char directoryReportPath[100];
char prepayReportPath[100];
char scheduleReportPath[100];

size_t arg_c = 0;
bool unitisvalid = false;

void get_prepaid_value(char*, char****, size_t t, size_t*);
void get_schedule_values(char* unitnumber, char**** data, size_t t, size_t* tu);
bool process_schedule_buffer(char**** data, size_t t, size_t* tu, char* bp, char* unitnumber);
bool process_ppbuffer(char****, size_t, size_t*, char*, char*);
int process_buffer(char****, char*, size_t*, size_t*, size_t, const char*);
size_t match_ledgerID(size_t*, size_t*, size_t*, char*,char****);

void extract_data_from_source(const char *argv[], char ****data, size_t* tc, size_t* tu)
{
	/* Data Directory Paths */
	sprintf(directoryReportPath, "%s%s", getenv("XDG_DATA_HOME"), "/bob/reports/Bronson Mini Storage - Directory.txt");
	sprintf(prepayReportPath, "%s%s", getenv("XDG_DATA_HOME"), "/bob/reports/Bronson Mini Storage - Prepaid Rent Liabilities.txt");
	sprintf(scheduleReportPath, "%s%s", getenv("XDG_DATA_HOME"), "/bob/reports/schedule.txt");

	/* Open the data source files and setup pointers */
	FILE *fp = fopen(directoryReportPath, "r");
	if (fp == NULL) {
		printf("Error: Unable to open Directory report. Make sure the filename is exactly \"Bronson Mini Storage - Directory.txt\"\n");
		exit(1);
	}

	fpos_t position;
	fgetpos(fp, &position);
	char * bp = NULL;
	int line_index;
	char buffer[SIZEOF_BUFFER];

	/* Begin processing CLI arguments (units passed to the program) */
	for (size_t arg=1; arg<arg_c; arg++) {
		fsetpos(fp, &position);
		line_index = 0;

		/* Process each line of the Directory report */
		while (fgets(buffer, SIZEOF_BUFFER, fp) != NULL) {
			bp = (char*)malloc(SIZEOF_BUFFER);
			buffer[SIZEOF_BUFFER-1] = '\0';
			memcpy(bp, buffer, SIZEOF_BUFFER);
	
			/* Parse each field of the Directory report, excluding the header line */
			if (line_index > 0 && process_buffer(data, bp, tc, tu, arg, argv[arg]) == 0) {
				free(bp);
				break;
			}
	
			line_index++;
			free(bp);
		}

		/* Error if unit wasn't matched in report */
		if (unitisvalid == false) {
			printf("Unit \"%s\" is either not currently rented or is invalid.\nNothing was done. Exiting...\n", argv[arg]);
			free_memory(data);
			exit(1);
		}
	}

	fclose(fp);

	return;
}


int process_buffer(char ****data, char * bp, size_t *tc, size_t *tu, size_t arg, const char *argv)
{
	char *token = NULL;
	size_t col = 0;
	size_t t = *tc-1;

	/* Proccess each tab delimited field of the buffer line of the Directory report */
	while((token = strsep(&bp, "\t")) != NULL) { 
		
		/* Reject if the first field doesn't match the unit number in Directory report */
		if (col == 0) { 
			if (strcmp(token, argv) != 0) {
				token = NULL;
				unitisvalid = false;
				return 1;
			}
			else
				unitisvalid = true;
		}

		/* Check if this unit is rented by an already processed tenant by matching the tenant ID */
		if (col == 3) {
			
			/* But only if this isn't the first tenant that's been processed */
			for (size_t i=1; i<=*tc && arg > 1; i++) {
				
				/* Add unit to existing tenant */
				if (strcmp(token, data[i-1][0][1]) == 0) {
					t = i-1;
					tu[t] += 1;
					break;
				}
				
				/* Add tenant */
				else if (i == *tc) {
					*tc += 1;
					t = *tc-1;
					tu[t] = 1;
					break;
				}
			}

			/* Only then copy the unit field to its appropriate position in the array */
			strcpy(data[t][tu[t]-1][0], argv);

			/* Get the period value from the schedule */
			get_schedule_values(data[t][tu[t]-1][0], data, t, tu);
		}

		/* Get prepaid rent */
		if (col == 166)
			get_prepaid_value(token, data, t, tu);

		/* Copy subsequent fields to array */
		switch (col) {
			case 3: // tenant id
				strcpy(data[t][tu[t]-1][1], token);
				break;
			case 9: // first name
				strcpy(data[t][tu[t]-1][2], token);
				break;
			case 11: // last name
				strcpy(data[t][tu[t]-1][3], token);
				break;
			case 12: // company
				strcpy(data[t][tu[t]-1][4], token);
				break;
			case 13: // addr1
				strcpy(data[t][tu[t]-1][5], token);
				break;
			case 14: // addr2
				strcpy(data[t][tu[t]-1][6], token);
				break;
			case 15: // city
				strcpy(data[t][tu[t]-1][7], token);
				break;
			case 16: // state
				strcpy(data[t][tu[t]-1][8], token);
				break;
			case 17: // zip
				strcpy(data[t][tu[t]-1][9], token);
				break;
			case 45: // email
				strcpy(data[t][tu[t]-1][10], token);
				break;
			case 135: // unit width
				strcpy(data[t][tu[t]-1][11], token);
				break;
			case 136: // unit length
				strcpy(data[t][tu[t]-1][12], token);
				break;
			case 166: // ledger ID
				strcpy(data[t][tu[t]-1][13], token);
				break;
			case 180: // rent
				strcpy(data[t][tu[t]-1][14], token);
				break;
			case 270: // Scheduled rent
				strcpy(data[t][tu[t]-1][15], token);
				break;
			case 271: // Scheduled rent date
				strcpy(data[t][tu[t]-1][16], token);
				break;
			case 223: // Rent balance
				strcpy(data[t][tu[t]-1][17], token);
				break;
			case 224: // Late fee balance
				strcpy(data[t][tu[t]-1][18], token);
				break;
			case 230: // NSF fee balance
				strcpy(data[t][tu[t]-1][19], token);
				break;
			case 232: // Lock cut fee balance
				strcpy(data[t][tu[t]-1][20], token);
				break;
			case 233: // Auction fee balance
				strcpy(data[t][tu[t]-1][21], token);
				break;
			case 243: // Other fee balance (certified letter)
				strcpy(data[t][tu[t]-1][22], token);
				break;
			case 245: // Paid thru date
				strcpy(data[t][tu[t]-1][23], token);
				break;
			default:
				break;
		}
		
		col++;
	}
	return 0;
}


void get_prepaid_value(char* ledgerID, char**** data, size_t t, size_t* tu)
{
	FILE *fp = fopen(prepayReportPath, "r");
	if (fp == NULL) {
		printf("Error: Unable to open Directory report. Make sure the filename is exactly \"Bronson Mini Storage - Prepaid Rent Liabilities.txt\"\n");
		exit(1);
	}
	const size_t SIZEOF_PPBUFFER = 450;
	fpos_t position;
	fgetpos(fp, &position);
	fsetpos(fp, &position);

	char * bp = NULL;
	int line_index = 0;
	char buffer[SIZEOF_PPBUFFER];

	/* Process each line of the file */
	while (fgets(buffer, SIZEOF_PPBUFFER, fp) != NULL) {
		bp = (char*)malloc(SIZEOF_PPBUFFER);
		buffer[SIZEOF_PPBUFFER-1] = '\0';
		memcpy(bp, buffer, SIZEOF_PPBUFFER);
	
		/* Process each field in the line */
		if (line_index > 0 && process_ppbuffer(data, t, tu, bp, ledgerID) == true) {
			free(bp);
			fclose(fp);
			return;
		}
	
		line_index++;
		free(bp);
	}

	strcpy(data[t][tu[t]-1][24], "0.00");
	fclose(fp);
	return;
}


bool process_ppbuffer(char**** data, size_t t, size_t* tu, char* bp, char* ledgerID)
{
	size_t col = 0;
	char* token = NULL;

	/* Process each token line */
	while((token = strsep(&bp, "\t")) != NULL) {
		if (col == 1 && strcmp(ledgerID, token) != 0) {
				token = NULL;
				return false;
			}

		// If matched, continue to column 10 and copy the prepayAmt
		if (col == 10) {
			strcpy(data[t][tu[t]-1][24], token);
			return true;
		}

		col++;
	}
	return false;
}


void get_schedule_values(char* unitnumber, char**** data, size_t t, size_t* tu)
{
	FILE *fp = fopen(scheduleReportPath, "r");
	if (fp == NULL) {
		printf("Error: Unable to open Directory report. Make sure the filename is exactly \"Bronson Mini Storage - Prepaid Rent Liabilities.txt\"\n");
		exit(1);
	}
	const size_t SIZEOF_PPBUFFER = 450;
	fpos_t position;
	fgetpos(fp, &position);
	fsetpos(fp, &position);

	char * bp = NULL;
	int line_index = 0;
	char buffer[SIZEOF_PPBUFFER];

	/* Process each line of the file */
	while (fgets(buffer, SIZEOF_PPBUFFER, fp) != NULL) {
		bp = (char*)malloc(SIZEOF_PPBUFFER);
		buffer[SIZEOF_PPBUFFER-1] = '\0';
		memcpy(bp, buffer, SIZEOF_PPBUFFER);
	
		/* Process each field in the line */
		if (process_schedule_buffer(data, t, tu, bp, unitnumber) == true) {
			free(bp);
			fclose(fp);
			return;
		}
	
		line_index++;
		free(bp);
	}

	strcpy(data[t][tu[t]-1][24], "0.00");
	fclose(fp);
	return;
}


bool process_schedule_buffer(char**** data, size_t t, size_t* tu, char* bp, char* unitnumber)
{
	size_t col = 0;
	char* token = NULL;

	/* Process each token line */
	while((token = strsep(&bp, ",")) != NULL) {
		if (col == 0 && strcmp(unitnumber, token) != 0) {
				token = NULL;
				return false;
			}

		// If matched...
		switch (col) {
			case 2: // Get the prepay period
				strcpy(data[t][tu[t]-1][25], token);
				break;
			case 6: // Get the advanced delivery value
				strcpy(data[t][tu[t]-1][26], token);
				return true;
			default:
				break;
		}

		col++;
	}
	return false;
}


char **** get_dataArray(void)
{
	char ****data = NULL;
	if ((data = (char****)malloc((arg_c)*sizeof(char****))) == NULL)
		mem_error();
	for (size_t i=0; i<arg_c; i++) {
		if ((data[i] = (char***)malloc((arg_c)*sizeof(char***))) == NULL)
			mem_error();
		for (size_t j=0; j<arg_c; j++) {
			if ((data[i][j] = (char**)malloc(FIELDS*sizeof(char**))) == NULL)
				mem_error();
			for (size_t k=0; k<FIELDS; k++) {
				if ((data[i][j][k] = (char*)malloc(MAX_CHAR)) == NULL)
					mem_error();
				data[i][j][k][MAX_CHAR-1] = '\0';
			}
		}
	}	
	return data;
}


void free_memory(char ****data)
{
	for (size_t i=0; i<arg_c; i++) {
		for (size_t j=0; j<arg_c; j++) {
			for (size_t k=0; k<FIELDS; k++) {
				free(data[i][j][k]);
			}
			free(data[i][j]);
		}
		free(data[i]);
	}
	free(data);
}


void mem_error(void)
{
	printf("data m(emory )alloc(ation) returned NULL.\nExiting...\n");
	exit(1);
}
