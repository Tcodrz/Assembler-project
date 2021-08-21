#include "error_handler.h"


void printError(Error error, Boolean * hasError)
{
    static char *filename;

    if (error.code == INITIALIZE)
    {
        filename = (char *)calloc(strlen(error.filename) + 1, sizeof(char));
        strcpy(filename, error.filename);
        filename[-1] = '\0';

        printf("Current File: \'%s\'\n\n", filename);
        return;
    }
    else if (error.code == FINISH_FILE)
    {
        if (filename)
        {
            free(filename);
        }
    }
    
    *hasError = TRUE;

    printf("\nERROR - ");

    switch (error.code)
    {
    case MISSING_FILENAME:
        printf("Please provid a valid filename");
        break;
    case CANT_FIND_FILE:
        printf("Cant find file '%s'", error.filename);
        break;
    case MAX_LINE_SIZE:
        printf("Line too long at file '%s' line %d", error.filename, error.lineNumber);
        break;
    case INVALID_LABEL_NAME:
        printf("Invalid label name %s at file '%s' line %d", error.message, filename, error.lineNumber);
        break;
    case INVALID_LABEL_POSITION:
        printf("Invalid label position at file '%s' line %d", filename, error.lineNumber);
        break;
    case INVALID_COMMAND_NAME:
        printf("Invalid command name -'%s' at file '%s' line %d", error.message, filename, error.lineNumber);
        break;
    case EMPTY_OPERAND:
        printf("Invalid operand at file '%s' line %d", filename, error.lineNumber);
        break;
    case INVALID_STRING:
        printf("Invalid string - \"%s\" at file %s line %d", error.message, filename, error.lineNumber);
        break;
    case LINE_ERROR:
        printf("Line %d at file '%s'", error.lineNumber, filename);
        break;
    default:
        printf("Unknown Error");
        break;
    }
    printf("\n");
}