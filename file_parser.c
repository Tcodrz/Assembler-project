#include "file_parser.h"

void printLine(Line *line);

void parseFile(char *fileName, Round round)
{
    int IC = INIT_IC;
    int DC = INIT_DC;
    int lineCounter = 0;
    char *lineText;
    Error *error = (Error *)calloc(1, sizeof(Error));
    Boolean lineHasError = FALSE;
    Line *line = (Line *)calloc(1, sizeof(line));

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

        while (lineText)
        {
            line->index = lineCounter;
            line->hasError = FALSE;
            lineCounter++;

            if (strlen(lineText) > LINE_SIZE)
            {
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

            line = calloc(1, sizeof(Line));
            line = parseLine(lineText, lineCounter);

            if (line->hasError)
            {
                error->code = LINE_ERROR;
                error->lineNumber = lineCounter;
                printError(*error, &lineHasError);
                lineText = readLine(file);
                continue;
            }

            if (line->isCommand)
            {
                if (line->label)
                {
                    /* put it in symbols table with attribute 'code' and value of IC  */
                    addLineToSymbolsTable(line->label, "code", IC);
                }
                /* analyze and validate operands data */

                /* code it in binary and put it in memory image */

                /* update IC */
                line->address = IC;
                IC += 4;
            }
            else if (line->isDirective)
            {
                if ((strcmp(line->directive->name, ".db") == 0) ||
                    (strcmp(line->directive->name, ".dh") == 0) ||
                    (strcmp(line->directive->name, ".dw") == 0))
                {
                    line->address = DC;
                    /* data drective */
                    if (line->label)
                    {
                        /* put it in symbols table with value DC 
                        */
                        addLineToSymbolsTable(line->label, "data", DC);
                    }

                    /* put it in data image - validat DATA and code it in binary 
                    */
                    addLineToDataImage(line);

                    /* increase DC and continue 
                    */
                    DC += (line->directive->byteMultiplier * line->numberOfArgs);
                }
                else if (strcmp(line->directive->name, ".asciz") == 0)
                {
                    /* string directive */
                    if (line->label)
                    {
                        /* put it in symbols table with value DC */
                        addLineToSymbolsTable(line->label, "data", DC);
                    }
                    /* put it in data image - validate DATA and code it in binary 
                    */

                    addLineToDataImage(line);
                    /* Increase DC and continue */
                    DC += (strlen(line->args[0]) -1);
                }
                else if (strcmp(line->directive->name, ".entry") == 0)
                {
                    /* entry */
                    /* continue - line will be handeled in second round */
                    lineText = readLine(file);
                    continue;
                }
                else if (strcmp(line->directive->name, ".extern") == 0)
                {
                    /* external */
                    /* put it's operand in symbols table with value 0 (zero) and attribute 'external' 
                    */
                    addLineToSymbolsTable(line->args[0], "external", 0);
                }
            }

            
            printLine(line);
            

            lineText = readLine(file);
        }
        printSymbolsTable();
        
        printDataImage();
        
    }
}

void printLine(Line *line)
{
    int i;

    printf("\n***************************************************\n");
    printf("line number: %d\n", line->index);
    printf("Address: %d\n", line->address);
    if (line->label)
    {
        printf("label: %s\n", line->label);
    }
    if (line->isCommand)
    {
        printf("command: %s\n", line->command->name);
    }
    else if (line->isDirective)
    {
        printf("directive: %s\n", line->directive->name);
    }
    printf("number of args: %d\n", line->numberOfArgs);
    if ((line->numberOfArgs) > 0)
    {
        printf("args: [ ");
        for (i = 0; i < (line->numberOfArgs); i++)
        {
            printf("%s", line->args[i]);
            if (i < (line->numberOfArgs - 1))
            {
                printf(", ");
            }
        }
        printf(" ]\n");
    }
}