#include "libs.h"




typedef enum {
    INITIALIZE = 100,
    FINISH_FILE,
    MISSING_FILENAME
}  ErrorCode;

typedef struct {
    ErrorCode code;
    char * message;
    char * filename;
    int lineNumber;
} Error;



void printError(Error error, Boolean * fileHasErrors);
