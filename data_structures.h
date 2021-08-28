#include "error_handler.h"

typedef struct
{
    char *name;
    char *attribute[2];
    int val;
    int attrCounter;
} Symbol;

typedef struct 
{
    char * labelName;
    int commandAddress;
} ExternalCommand;

typedef struct
{
    char *name;
    CommandType type;
    int funct;
    int opcode;
} Command;

typedef struct
{
    char *name;
    int byteMultiplier;
    int numberOfAllowedOpernads;
    DATA_TYPE dataType;
} Directive;

typedef struct 
{
    DATA_TYPE type;
    int valInt;
    char *valString;
} Operand;

typedef struct
{
    int index;
    int address;
    char *label;
    LineType type;
    Command *command;
    Directive *directive;
    char **args;
    int numberOfArgs;
    Boolean isCommand;
    Boolean isDirective;
    Boolean hasError;
    Operand **operands;
} Line;

typedef struct
{
    char c;
    int num;
    DATA_TYPE type;
} DataRow;

typedef struct
{
    DataRow *dataRows;
    int size;
    int startAddress;
} DataImage;

typedef struct
{
    int opcode;
    int rs;
    int rt;
    int rd;
    int funct;
    int reg;
    int immed;
    int address;
    int unused;
    CommandType type;
    int memoryAddress;
    int lineIndex;
} CodeRow;

static const Command COMMANDS[] = {
/*   Name     Type   Funct  Opcode */
    {"add",    R,     1,      0},
    {"sub",    R,     2,      0},
    {"and",    R,     3,      0},
    {"or",     R,     4,      0},
    {"nor",    R,     5,      0},
    {"move",   R,     1,      1},
    {"mvhi",   R,     2,      1},
    {"mvlo",   R,     3,      1},
    {"addi",   I,     0,     10},
    {"subi",   I,     0,     11},
    {"andi",   I,     0,     12},
    {"ori",    I,     0,     13},
    {"nori",   I,     0,     14},
    {"bne",    I,     0,     15},
    {"beq",    I,     0,     16},
    {"blt",    I,     0,     17},
    {"bgt",    I,     0,     18},
    {"lb",     I,     0,     19},
    {"sb",     I,     0,     20},
    {"lw",     I,     0,     21},
    {"sw",     I,     0,     22},
    {"lh",     I,     0,     23},
    {"sh",     I,     0,     24},
    {"jmp",    J,     0,     30},
    {"la",     J,     0,     31},
    {"call",   J,     0,     32},
    {"stop",   J,     0,     63},
};

static const Directive DIRECTIVES[] = {
    {".db", 1, -1, INT},
    {".dh", 2, -1, INT},
    {".dw", 4, -1, INT},
    {".asciz", 1, 1, STRING},
    {".entry", -1, 1, STRING},
    {".extern", -1, 1, STRING},
};



Error *addLineToSymbolsTable(char *name, char *attr, int value);

void addLineToCodeImage(Line *line);

Error * addLineToDataImage(Line *line);

void updateSymbolsTableAddress(int ic);

void updateDataTableAdress(int ic);

void updateJCommandAddress(int lineIndex, char * label);

Error *updateICommandAddress(int lineIndex, char *destinationLabel);

Boolean labelExists(char *label);

Boolean isRegistry(char * operand);

void printSymbolsTable();

void printDataImage();

void printCodeImage();

char * readLine(FILE * file);

Symbol *getSymbol(char * symbolName);

CodeRow * getCodeRow(int lineIndex);

Boolean lineIsEmpty(char * line);

Boolean lineIsComment(char * line);

Error * validateLabel(char * line);

Boolean validateCommandName(char * command);

Error * validateString(char * string);

Command *getCommand(char * commandName);

Directive *getDirective(char *commandName);

Boolean isReservedKeyword(char * string);

char * substring(char * string, int start, int end);

char *ltrim(char *string);

char *rtrim(char *string);

char *trim(char *string);

Boolean symbolExists(char * symbolName);

Boolean addAttributeToSYmbol(char * symbolName, char * attr);

void resetDataStructures();

Line *parseLine(char * line, int lineNumber);

void parseFile(char *fileName);