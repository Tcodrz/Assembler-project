#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>






/*************************************************************************/
/*                      GLOBAL VARIABLES AND TYPES                       */
/*************************************************************************/

#define MAX_LABEL_SIZE 31
#define COMMANDS_AMOUNT 27
#define DIRECTIVES_AMOUNT 6
#define ARG_SEPERATOR ','
#define LABEL_SYMBOL ':'
#define DIRECTIVE_SYMBOL '.'

typedef enum 
{
    INT = 600,
    STRING
} DATA_TYPE;

typedef enum
{
    COMMAND = 300,
    DIRECTIVE
} LineType;

typedef enum
{
    R = 400,
    I,
    J,
    DATA
} CommandType;

typedef enum {
    FIRST_ROUND = 200,
    SECOND_ROUND
} Round;

typedef enum
{
    FALSE = 0,
    TRUE = 1
} Boolean;

