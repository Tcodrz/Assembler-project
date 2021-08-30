#include "data_structures.h"

Operands *getOperands(char *text, int lineIndex);

/*
    Parse one line of assembly into a Line Struct
*/
Line *parseLine(char *line, int index)
{
    int i;
    char *commandName;
    Boolean lineHasLabel = FALSE;
    Boolean lineHasError = FALSE;
    int labelEndPosition = -1, lastPosition = -1;
    Operands *OPERANDS = calloc(1, sizeof(Operands));
    Command *command = calloc(1, sizeof(Command));
    Directive *directive = calloc(1, sizeof(Directive));
    Error *error = calloc(1, sizeof(Error));
    Line *parsedLine = calloc(1, sizeof(Line));

    lineHasLabel = FALSE;
    lineHasError = FALSE;
    parsedLine->index = index;
    parsedLine->hasError = FALSE;

    line = trim(line);


    /* Get Label */
    for (i = 0; i < strlen(line); i++)
    {
        if (line[i] == LABEL_SYMBOL)
        {
            lineHasLabel = TRUE;
            labelEndPosition = i + 1;
            break;
        }
    }

    if (lineHasLabel)
    {
        error = validateLabel(line);

        if (error->code == VALID_LABEL)
        {
            parsedLine->label = error->message;
        }
        else
        {
            error->lineNumber = index;
            printError(*error, &lineHasError);
        }
    }
    else
    {
        parsedLine->label = '\0';
    }

    /* if line has label cut label from line */
    if (labelEndPosition >= 0)
    {
        line = substring(line, labelEndPosition, MAX_LINE_SIZE);
    }

    line = trim(line);

    /* Get Command or Directive */
    i = 0;
    while (!isspace(line[i]) && i < strlen(line))
    {
        i++;
    }

    lastPosition = i;

    commandName = substring(line, 0, (i - 1));

    if (commandName[0] == DIRECTIVE_SYMBOL)
    {
        parsedLine->type = DIRECTIVE;
        parsedLine->isDirective = TRUE;
        directive = getDirective(commandName);
        if (directive)
            parsedLine->directive = directive;
    }
    else
    {
        parsedLine->type = COMMAND;
        parsedLine->isCommand = TRUE;
        command = getCommand(commandName);
        if (command)
            parsedLine->command = command;
    }

    if (!validateCommandName(commandName))
    {
        error->code = INVALID_COMMAND_NAME;
        error->lineNumber = index;
        printError(*error, &lineHasError);
        parsedLine->hasError = TRUE;
        return parsedLine;
    }

    line = trim(substring(line, lastPosition, MAX_LINE_SIZE));

    /* Get operands */
    OPERANDS = getOperands(line, index);

    if (strlen(line) < 1 || !OPERANDS)
    {
        parsedLine->operands = NULL;
        parsedLine->numberOfArgs = 0;
    }
    else
    {
        parsedLine->operands = OPERANDS->operands;
        parsedLine->numberOfArgs = OPERANDS->size;
        parsedLine->hasError = OPERANDS->hasError;
    }

    if (lineHasError)
    {
        parsedLine->hasError = TRUE;
    }

    return parsedLine;
}


/*
    Retruns an Operands object which contains the lines operands, 
    amount of operands and error flag
*/
Operands *getOperands(char *text, int lineIndex)
{
    Operands *operands = calloc(1, sizeof(Operands));
    Operand *operand = calloc(1, sizeof(Operand));
    Error *error = calloc(1, sizeof(Error));
    Boolean hasError = FALSE;
    Boolean flag = FALSE;
    char *arg;
    int count = 0;
    int last = 0;
    int i, j;

    operands->hasError = FALSE;

    text = trim(text);

    if (strlen(text) < 1)
    {
        return NULL;
    }

    for (i = 0; i <= strlen(text); i++)
    {
        if (text[i] == ARG_SEPERATOR || i == strlen(text))
        {
            flag = TRUE;

            if (i == strlen(text))
            {
                arg = trim(substring(text, last, i - 1));
            }
            else
            {
                arg = substring(text, last, i - 1);
                arg = trim(arg);
                last = (i + 1);
            }

            if (strlen(arg) < 1)
            {
                error->code = EMPTY_OPERAND;
                error->lineNumber = lineIndex;
                printError(*error, &hasError);
            }

            if (isRegistry(arg))
            {
                arg = substring(arg, 1, strlen(arg));
                for (j = 0; j < strlen(arg); j++)
                {
                    if (!isdigit(arg[j]))
                    {
                        error->code = INVALID_OPERNAD;
                        error->lineNumber = lineIndex;
                        printError(*error, &hasError);
                    }
                }
                if (atoi(arg) < 0 || atoi(arg) > 31)
                {
                    error->code = REGISTRY_OUT_OF_RANGE;
                    error->lineNumber = lineIndex;
                    printError(*error, &hasError);
                }
                operand->type = INT;
                operand->valInt = atoi(arg);
            }
            else
            {
                if (atoi(arg))
                {
                    operand->type = INT;
                    operand->valInt = atoi(arg);
                }
                else
                {
                    operand->type = STRING;
                    operand->valString = arg;
                }
            }
        }
        if (flag)
        {
            if (!operands->operands)
            {
                operands->operands = calloc(1, sizeof(Operand));
            }
            else
            {
                operands->operands = (Operand *)realloc(operands->operands, (count * sizeof(Operand)) + (sizeof(Operand)));
            }

            operands->operands[count] = *operand;
            count++;
            flag = FALSE;
        }
    }

    if (hasError)
    {
        operands->hasError = TRUE;
    }

    operands->size = count;

    return operands;
}