#include "data_structures.h"


char *bin(unsigned n, int bytes);


Symbol *SymbolsTable[32];

DataImage **DATA_IMAGE_TABLE = NULL;
static int DATA_BLOCK_COUNTER = 0;

void addLineToSymbolsTable(char *name, char *attr, int value)
{
    static int symbolsCounter = 0;
    Symbol *symbol = (Symbol *)calloc(1, sizeof(Symbol));

    /* check if label exists */

    symbol->name = name;
    symbol->val = value;
    symbol->attrCounter = 0;
    symbol->attribute[symbol->attrCounter] = attr;
    symbol->attrCounter++;

    SymbolsTable[symbolsCounter] = symbol;
    symbolsCounter++;

    /*
    printSymbolsTable();
    */

    return;
}

void addLineToDataImage(Line *line)
{
    int bytes = (line->directive->byteMultiplier * 8);
    char *binary = calloc((bytes + 1), sizeof(char));
    int i, temp;
    DataImage *dataImage = calloc(1, sizeof(DataImage));
    DataRow *dataRow = calloc(1, sizeof(DataRow));

    /* Initializing data image block */
    dataImage->dataRows = calloc((line->numberOfArgs), sizeof(DataRow *));
    dataImage->size = line->numberOfArgs;
    dataImage->startAddress = line->address;

    /* translating each argument and save it in a data row -> save data row in current data image block */
    for (i = 0; i < line->numberOfArgs; i++)
    { 
        temp = atoi(line->args[i]);
        binary = bin(temp, (bytes - 1));
        dataRow = calloc(1, sizeof(DataRow));
        dataRow->machineCode = calloc((bytes + 1), sizeof(char));
        dataRow->machineCode = binary;
        dataImage->dataRows[i] = dataRow;
    }

    /* Initialize or Reallocate Data image table */
    if (!DATA_IMAGE_TABLE)
    {
        DATA_IMAGE_TABLE = calloc(1, sizeof(DataImage));
    }
    else
    {
        DATA_IMAGE_TABLE = realloc(DATA_IMAGE_TABLE, ((sizeof(DataImage) * DATA_BLOCK_COUNTER) + 1));
    }

    DATA_IMAGE_TABLE[DATA_BLOCK_COUNTER] = dataImage;
    DATA_BLOCK_COUNTER++;

    printDataImage();

    return;
}

void printDataImage()
{
    int i, j;
    DataImage *block;
    DataRow * row;
    printf("DATA BLOCK COUNTER: %d\n\n", DATA_BLOCK_COUNTER);
    printf("\nADDRESS\tCODE\n");
    for (i = 0; i < DATA_BLOCK_COUNTER; i++)
    {
        block = calloc(1, sizeof(DataImage));
        block = DATA_IMAGE_TABLE[i];
        printf("%d", block->startAddress);
        for (j = 0; j < block->size; j++)
        {
            row = calloc(1, sizeof(DataRow));
            row = block->dataRows[j];
            printf("\t%s\n", row->machineCode);
        }
    }

    printf("\n\n");
}

void printSymbolsTable()
{
    int i;
    printf("\n\n");
    printf("NAME\tATTR\tVAL\n");
    for (i = 0; i < 32; i++)
    {
        if (!SymbolsTable[i])
        {
            break;
        }
        printf("%s\t%s\t%d\n", SymbolsTable[i]->name, SymbolsTable[i]->attribute[0], SymbolsTable[i]->val);
    }
}

char *bin(unsigned n, int bytes)
{
    char *binary = calloc(33, sizeof(char));
    int j = 0;
    unsigned i;
    for (i = 1 << bytes; i > 0; i = i / 2)
    {
        binary[j] = (n & i) ? '1' : '0';
        j++;
    }
    binary[++j] = '\0';
    return binary;
}
