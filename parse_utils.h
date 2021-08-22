#include "data_structures.h"

#define LINE_SIZE 80
#define COMMENT ';'

char * readLine(FILE * file);

Boolean lineIsEmpty(char * line);

Boolean lineIsComment(char * line);

Error * validateLabel(char * line);

Error * validateCommandName(char * command);

Command *getCommand(char * commandName);

Directive * getDirective(char *commandName) ;

char * substring(char * string, int start, int end);

char *ltrim(char *string);

char *rtrim(char *string);

char *trim(char *string);