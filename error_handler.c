#include "error_handler.h"


void printError(Error error, Boolean * fileHasErrors)
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
    
    *fileHasErrors = TRUE;

    printf("\nERROR - ");
    switch (error.code)
    {
    case MISSING_FILENAME:
        printf("Please provid a valid filename\n");
        break;
    default:
        printf("Unknown Error");
        break;
    }
    printf("\n");
}