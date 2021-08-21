#include "line_parser.h"

Line *parseLine(char *line, int index)
{
    Error *error = (Error *)calloc(1, sizeof(Error));
    Line *parsedLine = (Line *)calloc(1, sizeof(Line));
    Command *command = (Command *)calloc(1, sizeof(Command));
    Directive *directive = (Directive *)calloc(1, sizeof(Directive));

    Boolean lineHasLabel;
    Boolean lineHasError;
    int labelEndPosition = -1, lastPosition = -1;
    char *commandName;
    char **args = (char **)calloc(1, sizeof(char *));
    char *arg = (char *)calloc(MAX_LINE_SIZE, sizeof(char));
    int numberOfArgs = 0;
    Boolean argSeperator = FALSE;
    int i, j;

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
    }
    else
    {
        parsedLine->type = COMMAND;
    }

    error = validateCommandName(commandName);

    if (error->code == VALID_COMMAND_NAME)
    {
        command = getCommand(commandName);
        directive = getDirective(commandName);
        parsedLine->directive = directive;
        parsedLine->command = command;
    }
    else
    {
        error->lineNumber = index;
        printError(*error, &lineHasError);
    }

    if (command->name && !directive->name)
    {
        parsedLine->isCommand = TRUE;
    }
    else if (directive->name && !command->name)
    {
        parsedLine->isDirective = TRUE;
    }

    line = trim(substring(line, lastPosition, MAX_LINE_SIZE));

    /* Get args */
    if (strlen(line) > 0)
    {
        numberOfArgs = 1;
        lastPosition = 0;
        i = 0;
        while (i < strlen(line))
        {
            if (line[i] == ARG_SEPERATOR)
            {
                if (argSeperator)
                {
                    error->code = EMPTY_OPERAND;
                    error->lineNumber = index;
                    printError(*error, &lineHasError);
                }
                else
                {
                    argSeperator = TRUE;
                    arg = substring(line, lastPosition, (i - 1));
                    lastPosition = (i + 1);

                    if (strlen(arg) > 0)
                    {
                        argSeperator = FALSE;
                        args[numberOfArgs - 1] = trim(arg);
                        numberOfArgs++;
                    }
                }
            }
            i++;
        }

        arg = substring(line, lastPosition, i);
        args[numberOfArgs - 1] = trim(arg);

        /* Validate operands not empty */
        for (i = 0; i < numberOfArgs; i++)
        {
            if (strlen(args[i]) < 1)
            {
                error->code = EMPTY_OPERAND;
                error->lineNumber = index;
                printError(*error, &lineHasError);
            }
            for (j = 0; j < strlen(args[i]); j++)
            {
                if (isspace(args[i][j]))
                {
                    error->code = EMPTY_OPERAND;
                    error->lineNumber = index;
                    printError(*error, &lineHasError);
                    break;
                }
            }
        }
        parsedLine->args = args;
    }
    parsedLine->numberOfArgs = numberOfArgs;

    if (lineHasError)
    {
        parsedLine->hasError = TRUE;
    }

    return parsedLine;
}
