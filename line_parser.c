#include "data_structures.h"

Line *parseLine(char *line, int index)
{
    int i;
    char *commandName;
    Boolean lineHasLabel = FALSE;
    Boolean lineHasError = FALSE;
    int labelEndPosition = -1, lastPosition = -1;
    int numberOfArgs = 0;
    char **args = NULL;
    char *arg = calloc(MAX_LABEL_SIZE, sizeof(char));
    Operand *operand = calloc(1, sizeof(Operand));
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

    /* Get args */
    if (strlen(line) > 0)
    {
        numberOfArgs = 1;
        i = 0;

        while (i < strlen(line))
        {
            if (line[i] == ARG_SEPERATOR)
            {
                numberOfArgs++;
            }
            i++;
        }

        args = calloc(numberOfArgs, sizeof(char));
        arg = strtok(line, ",");
        args[0] = arg;
        i = 1;
        while (i < numberOfArgs)
        {
            arg = strtok(NULL, ",");
            args[i] = arg;
            i++;
        }

        parsedLine->operands = calloc(numberOfArgs, sizeof(Operand));
        parsedLine->numberOfArgs = numberOfArgs;

        for (i = 0; i < numberOfArgs; i++)
        {
            operand = calloc(1, sizeof(Operand));
            args[i] = trim(args[i]);
            if (isRegistry(args[i]))
            {
                args[i] = substring(args[i], 1, strlen(args[i]));
                operand->valInt = atoi(args[i]);
                operand->type = INT;
            }
            else if (atoi(args[i]))
            {
                operand->valInt = atoi(args[i]);
                operand->type = INT;
            }
            else
            {
                operand->type = STRING;
                operand->valString = calloc(strlen(args[i]), sizeof(char));
                strcpy(operand->valString, args[i]);
            }
            parsedLine->operands[i] = operand;
        }
    }

    parsedLine->args = args;

    if (lineHasError)
    {
        parsedLine->hasError = TRUE;
    }


    return parsedLine;
}
