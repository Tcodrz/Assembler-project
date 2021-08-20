#include "libs.h"




typedef enum {
    INITIALIZE = 100,
    FINISH_FILE,
    MISSING_FILENAME,
    CANT_FIND_FILE,
    MAX_LINE_SIZE,
    INVALID_LABEL_NAME,
    INVALID_LABEL_POSITION,
    VALID_LABEL
}  ErrorCode;

typedef struct {
    ErrorCode code;
    char * message;
    char * filename;
    int lineNumber;
} Error;


/*  
    Takes an error object and a pointer to boolean value
    prints an error and set the pointer to TRUE
*/
void printError(Error error, Boolean * hasError);
