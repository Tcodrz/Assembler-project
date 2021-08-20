#include "error_handler.h"

#define LINE_SIZE 80
#define COMMENT ';'

/* reads one line and return it as a string */
char * readLine(FILE * file);

/* return true if line is empty */
Boolean lineIsEmpty(char * line);

/* return true if line is a comment */
Boolean lineIsComment(char * line);

/* validate label name is not a keyword and label position */
Error * validateLabel(char * line);

/* takes a string, start and end points and returns a new substring */
char * substring(char * string, int start, int end);

char *ltrim(char *string);

char *rtrim(char *string);

char *trim(char *string);