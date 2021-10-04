// Standard required headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Header file
#include "gen_helper.h"

/*
INPUT: Error message to be used when exiting
OUTPUT: None
ACTIONS: Prints error message to stderr with red "ERROR: "
text, and exits program with EXIT_FAILURE
*/
void exitWithError(char* errorMsg) {
	fprintf(stderr, ERROR_TAG);
	fprintf(stderr, "%s", errorMsg);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

/*
INPUT: Error message to be used when exiting
OUTPUT: None
ACTIONS: Prints error message to stderr with red "ERROR: "
text, and exits program with EXIT_FAILURE
*/
char* filenameExt(char *filename) {
    char* ext = strrchr(filename, '.');
    if (!ext) return "";
    return ext;
}