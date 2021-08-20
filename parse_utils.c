#include "parse_utils.h"

static const char * RESERVED_KEYWORDS[27] = { "add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo", "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt", "lb", "sb", "lw", "sw", "lh", "sh", "jmp", "la", "call", "stop" };

char *readLine(FILE *file)
{
    int count = 0;
    char c;
    char *line = calloc(LINE_SIZE, sizeof(char));
    while ((c = fgetc(file)) != '\n')
    {
        if (c == EOF) {
            return NULL;
        }
        strncat(line, &c, 1);
        count++;
    }
    return line;
}

Boolean lineIsEmpty(char * line) 
{
    Boolean lineNotEmpty = FALSE;
    int i;
    for (i = 0; i < strlen(line); i++) 
    {
        if (!isspace(line[i]))
        {
            lineNotEmpty = TRUE;
            break;
        }
    }
    return !lineNotEmpty;
}

Boolean lineIsComment(char * line) 
{
    int i;
    for (i = 0; i < strlen(line); i++) {
        if (line[i] == COMMENT)
        {
            return TRUE;
        }
    }

    return FALSE;
}

Error * validateLabel(char * line) 
{
    Error * error = (Error *) calloc(1, sizeof(Error));
    char * label = (char *) calloc(MAX_LABEL_SIZE, sizeof(char));
    Boolean validLabelPosition = FALSE;
    Boolean validLabelName = FALSE;

    int i, j;
    for (i = 0; i < strlen(line); i++) 
    {
        if (line[i] == ':')
        {
            label = substring(line, 0, i-1);
        }
    }

    label = trim(label);

    /* check label position */
    for (j = 0; j < strlen(label); j++) {
        if (isspace(label[j]))
        {
            error->code = INVALID_LABEL_POSITION;
            return error;
        }
    }

    if (j == strlen(label))
    {
        validLabelPosition = TRUE;
    }

    /* check label contains only alphanumeric characters */
    for (i = 0; i < strlen(label); i++)
    {
        if ((!isalpha(label[i])) && (!isdigit(label[i]))) {
            validLabelName = FALSE;
            error->code = INVALID_LABEL_NAME;
            error->message = label;
        }
    }

    if (i == strlen(label))
    {
        validLabelName = TRUE;
    }

    /* check label is not a reserved keyword */
    for (i = 0; i < 27; i++) 
    {
        if (strcmp(label, RESERVED_KEYWORDS[i]) == 0) 
        {
            validLabelName = FALSE;
            error->code = INVALID_LABEL_NAME;
            error->message = label;
        }
    }

    if (validLabelName && validLabelPosition) 
    {
        error->code = VALID_LABEL;
    }

    return error;


}

char * substring(char * string, int start, int end)
{
    char * s = (char*) calloc(MAX_LABEL_SIZE, sizeof(char));

    int i = 0;
    while (start <= end) {
        s[i] = string[start];
        start++;
        i++;
    }
    s[i] = '\0';
    
    return s;
}


/* 
    trim left side of the string 
*/
char *ltrim(char *s)
{
    while (isspace(*s))
        s++;
    return s;
}

/* 
    trim right side of the string 
*/
char *rtrim(char *s)
{
    char *back = s + strlen(s);
    while (isspace(*--back))
        ;
    *(back + 1) = '\0';
    return s;
}

/* 
    Trim a string from all whitsapaces
*/
char *trim(char *s)
{
    return rtrim(ltrim(s));
}