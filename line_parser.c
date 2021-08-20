#include "line_parser.h"

Line *parseLine(char *line, int index)
{
    Error * error = (Error *) calloc(1, sizeof(Error));
    Line *parsedLine = (Line *)calloc(1, sizeof(Line));
    
    Boolean lineHasLabel;
    Boolean validLabel;
    Boolean lineHasError;
    int i;

    lineHasLabel = FALSE;
    validLabel = FALSE;
    lineHasError = FALSE;

    line = trim(line);

    for (i = 0; i < strlen(line); i++)
    {
        if (line[i] == ':')
        {
            lineHasLabel = TRUE;
        }
    }


    error = validateLabel(line);

    if (error->code != VALID_LABEL) {
        error->lineNumber = index;
        printError(*error, &lineHasError);
    }



    return parsedLine;
}
