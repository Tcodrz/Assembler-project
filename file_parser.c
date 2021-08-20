#include "file_parser.h"


void parseFile(char *fileName, Round round)
{
    int IC = INIT_IC;
    int DC = INIT_DC;
    int lineCounter = 1;
    char * lineText;
    Error *error = (Error *)calloc(1, sizeof(Error));
    Boolean lineHasError = FALSE;
    Line * line = (Line *) calloc(1, sizeof(line));

    FILE *file = NULL;
    file = fopen(fileName, "r");

    if (file == NULL)
    {
        error->code = CANT_FIND_FILE;
        error->filename = fileName;
        printError(*error, &lineHasError);
    }
    else
    {
        lineText = readLine(file);

        while (lineText) {

            line->index = lineCounter;
            lineCounter++;

            if (strlen(lineText) > LINE_SIZE) {
                error->code = MAX_LINE_SIZE;
                error->filename = fileName;
                printError(*error, &lineHasError);
                lineText = readLine(file);
                continue;
            }

            if (lineIsEmpty(lineText) || lineIsComment(lineText))
            {
                lineText = readLine(file);
                continue;
            }

            line = parseLine(lineText, lineCounter);

            lineText = readLine(file);
        }
    }
}