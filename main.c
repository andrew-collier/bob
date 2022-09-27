#include "data.h"
#include "groff.h"

int main (const int argc, const char* argv[])
{
	/* Check if any units were passed */
	if (argc < 2) {
		printf("No units were specified. Nothing was done. Exiting...\n");
		exit(1);
	}
	else {
		arg_c = argc; // make argc global
	}

	/* Create data array */
	char**** data = get_dataArray();

	/* Initialize variables for the total number of tenants */
	size_t tc = 1;
	size_t* tcPtr = &tc;

	/* Initialize variables for the total number of units rented by each tenant */
	size_t tu[argc-1];
	size_t* tuPtr = tu;
	tu[0] = 1;
	for (int i=1; i<argc-1; i++)
		tu[i] = 0;

	/* Extract values from files and populate data array */
	extract_data_from_source(argv, data, tcPtr, tuPtr);

	/* Test printing */
	//for (size_t tenant=0; tenant<tc; tenant++) {
	//	printf("%s %s:\n", data[tenant][0][2], data[tenant][0][3]);
	//	for (size_t unit=0; unit<tu[tenant]; unit++) {
	//		printf("\t%s -- %s\n", data[tenant][unit][0], data[tenant][unit][22]);
	//	}
	//}

	/* Generate roff documents */
	mkroff(data, tcPtr, tu);

	/* Free data array */
	free_memory(data);

	/* Exit program successfully */
	return 0;
}


