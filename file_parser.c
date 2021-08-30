#include "data_structures.h"

Error *secondRound(char *filename);

Line **PARSED_LINES = NULL;
static int LINE_COUNTER = 0;

/*
    Parse one file 
    scan the file once and build a list of parsed lines
    if no errors during the scan it will call second round function with the list of parsed commands
*/
void parseFile(char *fileName)
{
    int IC = INIT_IC;
    int DC = INIT_DC;
    int lineCounter = 0;
    Boolean lineHasError = FALSE;
    char *lineText = calloc(LINE_SIZE, sizeof(char));
    char *rawLine = calloc(LINE_SIZE, sizeof(char));
    Error *error = (Error *)calloc(1, sizeof(Error));
    Line *line = (Line *)calloc(1, sizeof(line));
    Line *parsedLine = (Line *)calloc(1, sizeof(line));

    FILE *file = NULL;
    file = fopen(fileName, "r");

    if (file == NULL)
    {
        error->code = CANT_FIND_FILE;
        error->filename = fileName;
        printError(*error, &lineHasError);
        return;
    }
    else
    {
        while (fgets(rawLine, (LINE_SIZE * 2), file) != NULL)
        {
            lineCounter++;
            lineText = trim(rawLine);

            if (strlen(lineText) > LINE_SIZE)
            {
                error->code = MAX_LINE_SIZE;
                error->lineNumber = lineCounter;
                printError(*error, &lineHasError);
                continue;
            }

            if (lineIsEmpty(lineText) || lineIsComment(lineText))
            {
                continue;
            }


            line = parseLine(lineText, lineCounter); /* text beeing parsed into Line Struct */
            line->index = lineCounter;

            if (line->hasError)
            {
                continue;
            }

            if (!PARSED_LINES)
            {
                PARSED_LINES = calloc(1, sizeof(Line));
            }
            else
            {
                PARSED_LINES = realloc(PARSED_LINES, (LINE_COUNTER * sizeof(Line)) + 1);
            }

            if (line->isCommand)
            {
                if (line->label)
                {
                    error = addLineToSymbolsTable(line->label, "code", IC);
                    if (error->code == LABEL_EXISTS)
                    {
                        error->lineNumber = line->index;
                        printError(*error, &lineHasError);
                        continue;
                    }
                }

                line->address = IC;
                IC += 4;

                error = addLineToCodeImage(line);
                if (error->code != SUCCESS)
                {
                    printError(*error, &lineHasError);
                    continue;
                }
            }
            else if (line->isDirective)
            {

                /* data drective  */
                if ((strcmp(line->directive->name, ".db") == 0) ||
                    (strcmp(line->directive->name, ".dh") == 0) ||
                    (strcmp(line->directive->name, ".dw") == 0))
                {
                    line->address = DC;
                    if (line->label)
                    {
                        addLineToSymbolsTable(line->label, "data", DC);
                    }
                    error = addLineToDataImage(line);

                    if (error->code == INVALID_STRING)
                    {
                        printError(*error, &lineHasError);
                        continue;
                    }

                    DC += (line->directive->byteMultiplier * line->numberOfArgs);
                }
                else if (strcmp(line->directive->name, ".asciz") == 0)
                {
                    error = validateString(line->operands[0].valString);

                    if (error->code == INVALID_STRING)
                    {
                        error->lineNumber = line->index;
                        error->message = line->operands[0].valString;
                        printError(*error, &lineHasError);
                        continue;
                    }
                    else
                    {
                        if (line->label)
                        {
                            addLineToSymbolsTable(line->label, "data", DC);
                        }
                        addLineToDataImage(line);

                        DC += (strlen(line->operands[0].valString) - 1);
                    }
                }
                else if (strcmp(line->directive->name, ".entry") == 0)
                {
                    /* continue - line will be handeled in second round */
                    parsedLine = line;
                    PARSED_LINES[LINE_COUNTER] = parsedLine;
                    LINE_COUNTER++;
                    continue;
                }
                else if (strcmp(line->directive->name, ".extern") == 0)
                {
                    /*  if external directive 
                         put it's operand in symbols table with value 0 (zero) 
                         and attribute 'external' */
                    addLineToSymbolsTable(line->operands[0].valString, "external", 0);
                }
            }

            parsedLine = line;
            PARSED_LINES[LINE_COUNTER] = parsedLine;
            LINE_COUNTER++;
        }
        updateSymbolsTableAddress(IC);
        updateDataTableAdress(IC);

        if (lineHasError)
        {
            return;
        }
        error = secondRound(fileName);
        if (error->code != SUCCESS)
        {
            printError(*error, &lineHasError);
            return;
        }

        printMemoryImage(IC, DC, fileName);

        resetDataStructures();
        free(PARSED_LINES);
        PARSED_LINES = NULL;
        LINE_COUNTER = 0;
    }
}

Error *secondRound(char *filename)
{
    int i = 0;
    Error *error = calloc(1, sizeof(Error));
    Line *line;
    Boolean hasError = FALSE;


    while (i < LINE_COUNTER)
    {
        line = PARSED_LINES[i];

        if (line->label)
        {
            i++;
            continue;
        }
        if (line->isDirective)
        {
            if (strcmp(PARSED_LINES[i]->directive->name, ".entry") == 0)
            {

                if (symbolExists(PARSED_LINES[i]->operands[0].valString))
                {
                    addAttributeToSYmbol(PARSED_LINES[i]->operands[0].valString, "entry");
                    addLineToEntryList(PARSED_LINES[i]);
                }
                else
                {
                    error->code = SYMBOL_NOT_FOUND;
                    error->lineNumber = PARSED_LINES[i]->index;
                    printError(*error, &hasError);
                }

            }
            else
            {
                i++;
                continue;
            }
        }
        else if (PARSED_LINES[i]->isCommand)
        {
            /* update J type commands */
            if (PARSED_LINES[i]->command->type == J)
            {
                if (PARSED_LINES[i]->command->opcode == 63)
                {
                    i++;
                    continue;
                }
                else
                {
                    updateJCommandAddress(PARSED_LINES[i]->index, PARSED_LINES[i]->operands[0].valString);
                }
            }
            else if ((PARSED_LINES[i]->command->type == I))
            {

                /* update I type commands */
                if (((PARSED_LINES[i]->command->opcode >= 15) && (PARSED_LINES[i]->command->opcode <= 18)))
                {
                    error = updateICommandAddress(PARSED_LINES[i]->index, PARSED_LINES[i]->operands[2].valString);

                    if (error->code != SUCCESS)
                    {
                        error->lineNumber = PARSED_LINES[i]->index;
                        printError(*error, &hasError);
                        i++;
                        continue;
                    }
                }
            }
        }

        i++;
    }
    if (hasError)
    {
        printf("Found Errors...Stoping..\n");
        return error;
    }
    else
    {
        error->code = SUCCESS;
        return error;
    }
}
