#include "libs.h"

typedef enum {
    INITIALIZE = 100,
    FINISH_FILE,
    MISSING_FILENAME,
    CANT_FIND_FILE,
    MAX_LINE_SIZE,
    INVALID_LABEL_NAME,
    INVALID_LABEL_POSITION,
    VALID_LABEL,
    INVALID_COMMAND_NAME,
    VALID_COMMAND_NAME,
    EMPTY_OPERAND,
    INVALID_OPERNAD,
    LINE_ERROR,
    INVALID_STRING,
    VALID_STRING,
    LABEL_EXISTS,
    SYMBOL_ADDED_TO_TABLE,
    SYMBOL_NOT_FOUND,
    CODE_ROW_NOT_FOUND,
    EXTERNAL_LABEL_IN_CONDITIONAL_CMD,
    REGISTRY_OUT_OF_RANGE,
    NUMBER_OF_OPERANDS,
    SUCCESS
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
