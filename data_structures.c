#include "data_structures.h"

char *numberToBinary(unsigned n, int bytes);
char *charToBinary(char c);

Symbol *SymbolsTable[32];

DataImage **DATA_IMAGE_TABLE = NULL;
static int DATA_BLOCK_COUNTER = 0;

void addLineToSymbolsTable(char *name, char *attr, int value)
{
    static int symbolsCounter = 0;
    Symbol *symbol = (Symbol *)calloc(1, sizeof(Symbol));

    /* TODO: check if label exists */

    symbol->name = name;
    symbol->val = value;
    symbol->attrCounter = 0;
    symbol->attribute[symbol->attrCounter] = attr;
    symbol->attrCounter++;

    SymbolsTable[symbolsCounter] = symbol;
    symbolsCounter++;

    return;
}

void addLineToDataImage(Line *line)
{

    int i, temp, count;
    DataImage *dataImage = calloc(1, sizeof(DataImage));
    DataRow *dataRow = calloc(1, sizeof(DataRow));
    int bytes = (line->directive->byteMultiplier * 8);
    char *binary = calloc((bytes + 1), sizeof(char));
    char *string = calloc(strlen(line->args[0]), sizeof(char));

    if (line->directive->dataType == STRING)
    {
        printf("\n\n\n~!~\t\tfound a string\t\t~!~\n\n");
        i = 0;
        count = 0;
        while (i < strlen(line->args[0]))
        {
            if (line->args[0][i] != '\"')
            {
                string[count] = line->args[0][i];
                count++;
            }
            i++;
        }
        count++;
        string[count] = '\0';
        count = 0;
        dataImage->size = strlen(string);
        dataImage->startAddress = line->address;
        dataImage->dataRows = calloc(strlen(string), sizeof(DataRow));

        for (i = 0; i < (strlen(string)); i++)
        {
            dataRow = calloc(1, sizeof(DataRow));
            dataRow->machineCode = calloc(9, sizeof(char));
            dataRow->machineCode = charToBinary(string[i]);
            dataImage->dataRows[i] = dataRow;
        }
    }
    else if (line->directive->dataType == INT)
    {

        /* Initializing data image block */
        dataImage->dataRows = calloc((line->numberOfArgs), sizeof(DataRow *));
        dataImage->size = line->numberOfArgs;
        dataImage->startAddress = line->address;

        /* translating each argument and save it in a data row -> save data row in current data image block */
        for (i = 0; i < line->numberOfArgs; i++)
        {
            /* TODO: Validate argument matches data tpe size */
            temp = atoi(line->args[i]);
            binary = numberToBinary(temp, (bytes - 1));
            dataRow = calloc(1, sizeof(DataRow));
            dataRow->machineCode = calloc((bytes + 1), sizeof(char));
            dataRow->machineCode = binary;
            dataImage->dataRows[i] = dataRow;
        }
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

    return;
}

void printDataImage()
{
    int i, j;
    DataImage *block;
    DataRow *row;
    printf("\n\nDATA BLOCK COUNTER: %d\n\n", DATA_BLOCK_COUNTER);
    printf("ADDRESS\tCODE\n");
    for (i = 0; i < DATA_BLOCK_COUNTER; i++)
    {
        printf("\bBLOCK %d\n\n", i);
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

char *numberToBinary(unsigned n, int bytes)
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

char *charToBinary(char c)
{
    int i, bin;
    char ch = c;
    char *binary = calloc(9, sizeof(char));

    if (!isalpha(c))
    {
        return NULL;
    }

    for (i = 0; i < 8; i++)
    {
        bin = ((ch << i) & 0x80) ? 1 : 0;
        binary[i] = bin + '0';
    }

    /*
    printf("%s\n", binary);
*/
    binary[9] = '\0';

    return binary;
}
