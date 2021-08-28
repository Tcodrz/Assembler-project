#include "data_structures.h"

char *numberToBinary(int number, int numberOfBits);
char *charToBinary(char c);
int getLabelAddress(char *label);
char *numberToBinaryV2(int n, int bytes);
Boolean isExternalSymbol(char *symbolName);
long convertCodeRowToLong(char *array[]);
unsigned int binaryToUnsignedInt(const char *binary);
void bin2Hex(char *str, FILE *);

Symbol **SYMBOL_TABLE = NULL;
static int SYMBOLS_COUNTER = 0;

DataImage *DATA_IMAGE_TABLE = NULL;
static int DATA_BLOCK_COUNTER = 0;

CodeRow **CODE_IMAGE_TABLE = NULL;
static int CODE_ROW_COUNTER = 0;

ExternalCommand **EXTERNAL_COMMANDS = NULL;
static int EXTERNAL_COMMANDS_COUNTER = 0;

ExternalCommand **ENTRY_COMMANDS = NULL;
static int ENTRY_COMMAND_COUNTER = 0;

/* Add line with label to symbols table */
Error *addLineToSymbolsTable(char *name, char *attr, int value)
{
    Error *error = (Error *)calloc(1, sizeof(Error));
    Symbol *symbol = (Symbol *)calloc(1, sizeof(Symbol));

    if (labelExists(name))
    {
        error->code = LABEL_EXISTS;
        return error;
    }

    symbol->name = name;
    symbol->val = value;
    symbol->attrCounter = 0;
    symbol->attribute[symbol->attrCounter] = attr;
    symbol->attrCounter++;

    if (!SYMBOL_TABLE)
    {
        SYMBOL_TABLE = calloc(1, sizeof(Symbol));
    }
    else
    {
        SYMBOL_TABLE = realloc(SYMBOL_TABLE, (sizeof(Symbol) * SYMBOLS_COUNTER) + 1);
    }

    SYMBOL_TABLE[SYMBOLS_COUNTER] = symbol;
    SYMBOLS_COUNTER++;

    error->code = SYMBOL_ADDED_TO_TABLE;

    return error;
}

/* add command lines to code image */
void addLineToCodeImage(Line *line)
{
    int i;
    int lastSixBits = 0;
    int destination;
    CodeRow *codeRow = calloc(1, sizeof(CodeRow));
    Symbol *symbol = calloc(1, sizeof(Symbol));

    codeRow->lineIndex = line->index;
    if (line->command->type == R)
    {
        if (line->command->opcode == 0)
        {
            codeRow->rs = line->operands[0]->valInt;
            codeRow->rt = line->operands[1]->valInt;
            codeRow->rd = line->operands[2]->valInt;
        }
        else if (line->command->opcode == 1)
        {
            codeRow->rs = line->operands[1]->valInt;
            codeRow->rd = line->operands[0]->valInt;
            codeRow->rt = 0;
        }

        codeRow->opcode = line->command->opcode;
        codeRow->funct = line->command->funct;
        codeRow->unused = lastSixBits;
        codeRow->type = R;
        codeRow->memoryAddress = line->address;
    }
    else if (line->command->type == I)
    {
        if (line->command->opcode >= 10 && line->command->opcode <= 14)
        { /* handle addi, subi, ori, nori commands */
            codeRow->rs = line->operands[0]->valInt;
            codeRow->rt = line->operands[2]->valInt;
            codeRow->immed = line->operands[1]->valInt;
        }
        else if (line->command->opcode >= 14 && line->command->opcode <= 18)
        { /* handle bne, beq, blt, bgt commands */
            codeRow->rs = line->operands[0]->valInt;
            codeRow->rt = line->operands[1]->valInt;
            destination = getLabelAddress(line->operands[2]->valString); /* get the address for the destination label */
            if (destination < 0)                                         /* if not found symbol immed = 0 */
            {                                                            /* immed = destination from this line address to third opernad label address */
                codeRow->immed = 0;
            }
            else
            {
                codeRow->immed = (line->address - destination);
            }
        }
        else if (line->command->opcode >= 19 && line->command->opcode <= 24)
        { /* handle lb, sb, lw, sw, lh, sh commands */
            codeRow->rs = line->operands[0]->valInt;
            codeRow->rt = line->operands[2]->valInt;
            codeRow->immed = line->operands[1]->valInt;
        }

        codeRow->opcode = line->command->opcode;
        codeRow->type = I;
        codeRow->memoryAddress = line->address;
    }

    else if (line->command->type == J)
    {

        if (line->command->opcode == 63)
        {
            codeRow->reg = 0;
            codeRow->address = 0;
        }
        else if (line->command->opcode >= 30 && line->command->opcode <= 32)
        {
            if (line->operands[0]->type == INT)
            {
                codeRow->reg = 1;
                codeRow->address = line->operands[0]->valInt;
            }
            else if (line->operands[0]->type == STRING)
            {
                codeRow->reg = 0;
                symbol = getSymbol(line->operands[0]->valString);

                if (!symbol || isExternalSymbol(line->operands[0]->valString))
                {
                    codeRow->address = 0;
                }
                else
                {
                    codeRow->address = symbol->val;
                }
            }
        }
        codeRow->opcode = line->command->opcode;
        codeRow->type = J;
        codeRow->memoryAddress = line->address;
    }

    if (!CODE_IMAGE_TABLE)
    {
        CODE_IMAGE_TABLE = calloc(1, sizeof(CodeRow));
    }
    else
    {
        CODE_IMAGE_TABLE = realloc(CODE_IMAGE_TABLE, (CODE_ROW_COUNTER * sizeof(CodeRow)) + 1);
    }

    CODE_IMAGE_TABLE[CODE_ROW_COUNTER] = codeRow;
    CODE_ROW_COUNTER++;
}

/* add directive line to data image */
Error *addLineToDataImage(Line *line)
{
    int i;
    char *str = NULL;
    DataImage *block = calloc(1, sizeof(DataImage));
    Error *error = calloc(1, sizeof(Error));

    block->byteMulti = line->directive->byteMultiplier;

    if (line->directive->dataType == STRING)
    {
        str = line->operands[0]->valString;

        str = substring(str, 1, strlen(str) - 2);
        block->size = (int)(strlen(str) + 1);
        block->startAddress = line->address;
        block->dataRows = calloc(strlen(str) + 1, sizeof(DataRow));
        for (i = 0; i < strlen(str); i++)
        {
            block->dataRows[i].type = STRING;
            block->dataRows[i].c = (char)str[i];
        }
        block->dataRows[i].type = INT;
        block->dataRows[i].num = 0;
    }
    else if (line->directive->dataType == INT)
    {
        block->size = line->numberOfArgs;
        block->startAddress = line->address;
        block->dataRows = calloc(block->size, sizeof(DataRow));

        for (i = 0; i < line->numberOfArgs; i++)
        {
            block->dataRows[i].type = INT;
            block->dataRows[i].num = line->operands[i]->valInt;
        }
    }

    if (!DATA_IMAGE_TABLE)
    {
        DATA_IMAGE_TABLE = (DataImage *)calloc(1, sizeof(DataImage));
    }
    else
    {
        DATA_IMAGE_TABLE = (DataImage *)realloc(DATA_IMAGE_TABLE, (DATA_BLOCK_COUNTER * sizeof(DataImage)) + (1 * sizeof(DataImage)));
    }

    DATA_IMAGE_TABLE[DATA_BLOCK_COUNTER] = *block;
    DATA_BLOCK_COUNTER++;

    error->code = SUCCESS;
    return error;
}

void updateDataTableAdress(int ic)
{
    int i;
    for (i = 0; i < DATA_BLOCK_COUNTER; i++)
    {
        DATA_IMAGE_TABLE[i].startAddress += ic;
    }
}

void updateSymbolsTableAddress(int ic)
{
    int i;
    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (strcmp(SYMBOL_TABLE[i]->attribute[0], "external") != 0)
        {
            SYMBOL_TABLE[i]->val += (SYMBOL_TABLE[i]->val < 100) ? ic : 0;
        }
    }
}

Error *updateICommandAddress(int lineIndex, char *destinationLabel)
{
    int i;
    int distance;
    Symbol *sym;
    Error *error = calloc(1, sizeof(Error));
    CodeRow *cr = calloc(1, sizeof(CodeRow));

    sym = getSymbol(destinationLabel);
    cr = getCodeRow(lineIndex);

    if (!sym->name)
    {
        error->code = SYMBOL_NOT_FOUND;
        return error;
    }
    if (!cr->lineIndex)
    {
        error->code = CODE_ROW_NOT_FOUND;
        return error;
    }

    distance = cr->memoryAddress > sym->val ? ((cr->memoryAddress - sym->val) * -1) : (sym->val - cr->memoryAddress);
    cr->address = distance;
    for (i = 0; i < CODE_ROW_COUNTER; i++)
    {
        if (CODE_IMAGE_TABLE[i]->lineIndex == lineIndex)
        {
            CODE_IMAGE_TABLE[i]->immed = distance;
            break;
        }
    }

    error->code = SUCCESS;
    return error;
}

void updateJCommandAddress(int lineIndex, char *label)
{
    int i;
    Symbol *symbol = NULL;
    ExternalCommand *exCommand = calloc(1, sizeof(ExternalCommand));

    for (i = 0; i < CODE_ROW_COUNTER; i++)
    {
        if ((CODE_IMAGE_TABLE[i]->lineIndex == lineIndex) && (!CODE_IMAGE_TABLE[i]->reg))
        {
            symbol = getSymbol(label);
            if (strcmp(symbol->attribute[0], "external") == 0)
            {
                exCommand->commandAddress = CODE_IMAGE_TABLE[i]->memoryAddress;
                exCommand->labelName = label;
                if (!EXTERNAL_COMMANDS)
                {
                    EXTERNAL_COMMANDS = calloc(1, sizeof(ExternalCommand));
                }
                else
                {
                    EXTERNAL_COMMANDS = realloc(EXTERNAL_COMMANDS, (EXTERNAL_COMMANDS_COUNTER * sizeof(ExternalCommand)) + 1);
                }
                EXTERNAL_COMMANDS[EXTERNAL_COMMANDS_COUNTER] = exCommand;
                EXTERNAL_COMMANDS_COUNTER++;
            }
            else
            {
                CODE_IMAGE_TABLE[i]->address = symbol->val;
            }
        }
    }
}

void addLineToEntryList(Line *line)
{
    int i;
    ExternalCommand *entryCommand = calloc(1, sizeof(ExternalCommand));

    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (strcmp(SYMBOL_TABLE[i]->name, line->operands[0]->valString) == 0)
        {
            entryCommand->commandAddress = SYMBOL_TABLE[i]->val;
            entryCommand->labelName = line->operands[0]->valString;
        }
    }
    if (!ENTRY_COMMANDS)
    {
        ENTRY_COMMANDS = calloc(1, sizeof(ExternalCommand));
    }
    else
    {
        ENTRY_COMMANDS = realloc(ENTRY_COMMANDS, (ENTRY_COMMAND_COUNTER * sizeof(ExternalCommand)) + 1);
    }
    ENTRY_COMMANDS[ENTRY_COMMAND_COUNTER] = entryCommand;
    ENTRY_COMMAND_COUNTER++;
}

Boolean symbolExists(char *symbolName)
{
    int i;
    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (strcmp(symbolName, SYMBOL_TABLE[i]->name) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean addAttributeToSYmbol(char *symbolName, char *attr)
{
    int i;
    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (strcmp(symbolName, SYMBOL_TABLE[i]->name) == 0)
        {
            SYMBOL_TABLE[i]->attribute[SYMBOL_TABLE[i]->attrCounter] = attr;
            SYMBOL_TABLE[i]->attrCounter++;
            return TRUE;
        }
    }
    return FALSE;
}

Symbol *getSymbol(char *symbolName)
{
    Symbol *s = NULL;
    int i;

    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (strcmp(symbolName, SYMBOL_TABLE[i]->name) == 0)
        {
            return SYMBOL_TABLE[i];
        }
    }
    return s;
}

CodeRow *getCodeRow(int lineIndex)
{
    int i;

    for (i = 0; i < CODE_ROW_COUNTER; i++)
    {
        if (CODE_IMAGE_TABLE[i]->lineIndex == lineIndex)
        {
            return CODE_IMAGE_TABLE[i];
        }
    }
    return NULL;
}

void resetDataStructures()
{
    SYMBOL_TABLE = NULL;
    SYMBOLS_COUNTER = 0;
    DATA_IMAGE_TABLE = NULL;
    DATA_BLOCK_COUNTER = 0;
    CODE_IMAGE_TABLE = NULL;
    CODE_ROW_COUNTER = 0;
}

void writeFiles(int ic, int dc, char *filename)
{
    int i, j, k, endOfCode;
    int b;
    int counter = 0;
    char *ramWord = calloc(33, sizeof(char));
    char *byte = calloc(9, sizeof(char));
    char *temp = calloc(33, sizeof(char));
    char *objectFilename, *entFilename, *extFilename;
    FILE *objectFile, *entFile, *extFile;

    /* remove the .as file extention */
    filename = substring(filename, 0, (strlen(filename) - 4));

    /* Create output files */
    objectFilename = calloc(strlen(filename) + 3, sizeof(char));
    entFilename = calloc(strlen(filename) + 4, sizeof(char));
    extFilename = calloc(strlen(filename) + 4, sizeof(char));

    strcat(objectFilename, filename);
    strcat(objectFilename, ".ob");

    strcat(entFilename, filename);
    strcat(entFilename, ".ent");

    strcat(extFilename, filename);
    strcat(extFilename, ".ext");

    objectFile = fopen(objectFilename, "w+");
    entFile = fopen(entFilename, "w+");
    extFile = fopen(extFilename, "w+");

    /* Print Code Image to Object file */
    fprintf(objectFile, "\t%d %d\n", (ic - 100), dc);
    for (i = 0; i < CODE_ROW_COUNTER; i++)
    {
        fprintf(objectFile, "0%d ", CODE_IMAGE_TABLE[i]->memoryAddress);
        if (CODE_IMAGE_TABLE[i]->type == R)
        {
            ramWord = calloc(33, sizeof(char));
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->opcode, 6);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->rs, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->rt, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->rd, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->funct, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->unused, 6);
            strcat(ramWord, temp);

            byte = substring(ramWord, 24, 32);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 16, 23);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 8, 15);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 0, 7);
            bin2Hex(byte, objectFile);
            fprintf(objectFile, "\n");
        }
        else if (CODE_IMAGE_TABLE[i]->type == I)
        {
            ramWord = calloc(33, sizeof(char));
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->opcode, 6);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->rs, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->rt, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->immed, 16);
            strcat(ramWord, temp);

            byte = substring(ramWord, 24, 32);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 16, 23);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 8, 15);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 0, 7);
            bin2Hex(byte, objectFile);

            fprintf(objectFile, "\n");
        }
        else if (CODE_IMAGE_TABLE[i]->type == J)
        {
            ramWord = calloc(33, sizeof(char));
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->opcode, 6);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->reg, 1);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE_TABLE[i]->reg, 25);
            strcat(ramWord, temp);

            byte = substring(ramWord, 24, 32);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 16, 23);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 8, 15);
            bin2Hex(byte, objectFile);
            byte = substring(ramWord, 0, 7);
            bin2Hex(byte, objectFile);
            fprintf(objectFile, "\n");
        }
    }

    /* Print Data Image to Object File */
    endOfCode = CODE_IMAGE_TABLE[i - 1]->memoryAddress + 4;
    fprintf(objectFile, "0%d ", endOfCode);

    for (i = 0; i < DATA_BLOCK_COUNTER; i++)
    {
        for (j = 0; j < DATA_IMAGE_TABLE[i].size; j++)
        {
            if (DATA_IMAGE_TABLE[i].dataRows[j].type == INT)
            {
                if ((counter % 4 == 0) && (counter != 0))
                {
                    fprintf(objectFile, "\n");
                    endOfCode += 4;
                    fprintf(objectFile, "0%d ", endOfCode);
                }

                temp = numberToBinary(DATA_IMAGE_TABLE[i].dataRows[j].num, (DATA_IMAGE_TABLE[i].byteMulti * 8));

                if (DATA_IMAGE_TABLE[i].byteMulti == 1)
                {
                    bin2Hex(temp, objectFile);
                    counter++;
                }
                else if (DATA_IMAGE_TABLE[i].byteMulti == 2)
                {
                    b = DATA_IMAGE_TABLE[i].byteMulti * 8;
                    for (k = 0; k < DATA_IMAGE_TABLE[i].byteMulti; k++)
                    {
                        byte = substring(temp, (b - 8), (b - 1));
                        bin2Hex(byte, objectFile);
                        counter++;
                        if ((counter % 4 == 0) && (counter != 0))
                        {
                            fprintf(objectFile, "\n");
                            endOfCode += 4;
                            fprintf(objectFile, "0%d ", endOfCode);
                        }
                        b /= 2;
                    }
                }
                else if (DATA_IMAGE_TABLE[i].byteMulti == 4)
                {
                    b = DATA_IMAGE_TABLE[i].byteMulti * 8;
                    for (k = 0; k < DATA_IMAGE_TABLE[i].byteMulti; k++)
                    {
                        byte = substring(temp, (b - 8), (b - 1));
                        bin2Hex(byte, objectFile);
                        counter++;
                        if ((counter % 4 == 0) && (counter != 0))
                        {
                            fprintf(objectFile, "\n");
                            endOfCode += 4;
                            fprintf(objectFile, "0%d ", endOfCode);
                        }
                        b /= 2;
                    }
                }
            }
            else if (DATA_IMAGE_TABLE[i].dataRows[j].type == STRING)
            {
                if ((counter % 4 == 0) && (counter != 0))
                {
                    fprintf(objectFile, "\n");
                    endOfCode += 4;
                    fprintf(objectFile, "0%d ", endOfCode);
                }
                temp = charToBinary(DATA_IMAGE_TABLE[i].dataRows[j].c);
                bin2Hex(temp, objectFile);
                counter++;
            }
        }
    }
    fprintf(objectFile, "\n");
    fclose(objectFile);

    for (i = 0; i < EXTERNAL_COMMANDS_COUNTER; i++)
    {

        fprintf(extFile, "%s ", EXTERNAL_COMMANDS[i]->labelName);
        fprintf(extFile, "0%d\n ", EXTERNAL_COMMANDS[i]->commandAddress);
    }

    for (i = 0; i < ENTRY_COMMAND_COUNTER; i++)
    {
        fprintf(entFile, "%s ", ENTRY_COMMANDS[i]->labelName);
        fprintf(entFile, "0%d\n", ENTRY_COMMANDS[i]->commandAddress);
    }
}

void bin2Hex(char *str, FILE *file)
{
    char *a = calloc(9, sizeof(char));
    strcpy(a, str);
    int b;
    int num = 0;
    do
    {
        b = *a == '1' ? 1 : 0;
        num = (num << 1) | b;
        a++;
    } while (*a);

    fprintf(file, "%02X ", num);
}

/**************************************************************************/
/*                      HELPER FUNCTIONS                                  */
/**************************************************************************/

Boolean isExternalSymbol(char *symbolName)
{
    int i, j;
    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (strcmp(symbolName, SYMBOL_TABLE[i]->name) == 0)
        {
            for (j = 0; j < SYMBOL_TABLE[i]->attrCounter; j++)
            {
                if (strcmp(SYMBOL_TABLE[i]->attribute[j], "external") == 0)
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

Boolean isRegistry(char *operand)
{
    return operand[0] == '$';
}

/* return a label's address from symbols table and -1 if no label found */
int getLabelAddress(char *label)
{
    int i;
    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (strcmp(SYMBOL_TABLE[i]->name, label) == 0)
        {
            return SYMBOL_TABLE[i]->val;
        }
    }
    return -1;
}

long convertCodeRowToLong(char *array[])
{
    int i;
    unsigned int x;

    for (i = 0; i < sizeof(array[0]); i++)
    {
        if (array[i])
        {
            x = binaryToUnsignedInt(array[i]);
            printf("%02X\t", x);
        }
        else
        {
            break;
        }
    }
    printf("\n");
    return x;
}

unsigned int binaryToUnsignedInt(const char *binary)
{
    unsigned int i = 0;
    unsigned int k = 1;
    unsigned int length;
    int c;

    length = strlen(binary);

    for (c = (length - 1); c >= 0; c--)
    {
        if (binary[c] != '0' && binary[c] != '1')
            return (0);
        if (binary[c] == '1')
        {
            i += k;
        }
        k *= 2;
    }
    return (i);
}

Boolean labelExists(char *label)
{
    int i;

    if (SYMBOLS_COUNTER < 1)
    {
        return FALSE;
    }

    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (strcmp(label, SYMBOL_TABLE[i]->name) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void printCodeImage()
{
    CodeRow *cr;
    int i;

    printf("\n/*************************************************************/\n");
    printf(" *                       CODE IMAGE                          *\n");
    printf("/*************************************************************/\n");

    for (i = 0; i < CODE_ROW_COUNTER; i++)
    {
        cr = CODE_IMAGE_TABLE[i];
        if (cr->type == R)
        {
            printf("%d\t\t%s\t%s\t%s\t%s\t%s\t%s\n", cr->memoryAddress, numberToBinary(cr->opcode, 6), numberToBinary(cr->rs, 5), numberToBinary(cr->rt, 5), numberToBinary(cr->rd, 5), numberToBinary(cr->funct, 5), numberToBinary(cr->unused, 6));
        }
        else if (cr->type == I)
        {
            printf("%d\t\t%s\t%s\t%s\t%s\n", cr->memoryAddress, numberToBinary(cr->opcode, 6), numberToBinary(cr->rs, 5), numberToBinary(cr->rt, 5), numberToBinary(cr->immed, 16));
        }
        else if (cr->type == J)
        {
            printf("%d\t\t%s\t%s\t%s\n", cr->memoryAddress, numberToBinary(cr->opcode, 6), numberToBinary(cr->reg, 1), numberToBinary(cr->address, 25));
        }
    }
}

void printDataImage()
{
    int i, j;
    DataImage block;
    DataRow row;
    printf("\n/*************************************************************/\n");
    printf(" *                       DATA IMAGE                          *\n");
    printf("/*************************************************************/\n");
    printf("ADDRESS\t\tCODE\n");
    for (i = 0; i < DATA_BLOCK_COUNTER; i++)
    {
        block = DATA_IMAGE_TABLE[i];
        printf("%d", block.startAddress);
        for (j = 0; j < block.size; j++)
        {
            row = block.dataRows[j];
            if (row.type == STRING)
            {
                printf("\t\t%s\n", charToBinary(row.c));
            }
            else if (row.type == INT)
            {
                printf("\t\t%s\n", numberToBinary(row.num, (block.byteMulti * 8)));
            }
        }
    }

}

void printSymbolsTable()
{
    int i, j;
    printf("\n/*************************************************************/\n");
    printf(" *                      SYMBOLS TABLE                        *\n");
    printf("/*************************************************************/\n");
    printf("NAME\tVAL\tATTR\n");
    for (i = 0; i < SYMBOLS_COUNTER; i++)
    {
        if (!SYMBOL_TABLE[i])
        {
            break;
        }
        printf("%s\t", SYMBOL_TABLE[i]->name);
        printf("%d\t", SYMBOL_TABLE[i]->val);

        for (j = 0; j < SYMBOL_TABLE[i]->attrCounter; j++)
        {
            printf("%s", SYMBOL_TABLE[i]->attribute[j]);
            if (j < (SYMBOL_TABLE[i]->attrCounter - 1))
            {
                printf(", ");
            }
        }
        printf("\n");
    }
}

char *numberToBinary(int n, int bytes)
{
    char *binary = calloc((bytes + 1), sizeof(char));
    int j = 0;
    unsigned int i;
    for (i = 1 << (bytes - 1); i > 0; i = i / 2)
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
