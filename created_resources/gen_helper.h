#ifndef GEN_HELPER_H
#define GEN_HELPER_H

// For tagging error text with red "ERROR: " declaration and newline
#define ERROR_TAG "\x1b[31mERROR:\x1b[0m "

/*
INPUT: Error message to be used when exiting
OUTPUT: None
ACTIONS: Prints error message to stderr with red "ERROR: "
text, and exits program with EXIT_FAILURE
*/
void exitWithError(char* errorMsg);

/*
INPUT: Error message to be used when exiting
OUTPUT: None
ACTIONS: Prints error message to stderr with red "ERROR: "
text, and exits program with EXIT_FAILURE
*/
char* filenameExt(char *filename);

#endif