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
} Line;

typedef struct {
    char * machineCode;
} DataRow;

typedef struct {
    DataRow ** dataRows;
    int size;
    int startAddress;
} DataImage;


static const Command COMMANDS[] = {
    {"add", R, 1, 0},
    {"sub", R, 2, 0},
    {"and", R, 3, 0},
    {"or", R, 4, 0},
    {"nor", R, 5, 0},
    {"move", R, 1, 1},
    {"mvhi", R, 2, 1},
    {"mvlo", R, 3, 1},
    {"addi", I, 0, 10},
    {"subi", I, 0, 11},
    {"andi", I, 0, 12},
    {"ori", I, 0, 13},
    {"nori", I, 0, 14},
    {"bne", I, 0, 15},
    {"beq", I, 0, 16},
    {"blt", I, 0, 17},
    {"bgt", I, 0, 18},
    {"lb", I, 0, 19},
    {"sb", I, 0, 20},
    {"lw", I, 0, 21},
    {"sw", I, 0, 22},
    {"lh", I, 0, 23},
    {"sh", I, 0, 24},
    {"jmp", I, 0, 30},
    {"la", I, 0, 31},
    {"call", I, 0, 32},
    {"stop", I, 0, 63},
};

static const Directive DIRECTIVES[] = {
    {".db",      1,     -1,     INT},
    {".dh",      2,     -1,     INT},
    {".dw",      4,     -1,     INT},
    {".asciz",   1,      1,  STRING},
    {".entry",  -1,      1,  STRING},
    {".extern", -1,      1,  STRING},
};

void addLineToSymbolsTable(char *name, char *attr, int value);

void addLineToDataImage(Line *line);

void printSymbolsTable();

void printDataImage();