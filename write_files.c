#include "data_structures.h"

void bin2Hex(char *str, FILE *);

/* Creates the output files and prints memory image to files */
void writeFiles(DataImage **DATA_IMAGE, int DATA_COUNTER, CodeRow **CODE_IMAGE,
                int CODE_COUNTER, ExternalCommand **EXTERNAL_CMDS,
                int EXTERNAL_COUNTER, ExternalCommand **ENTRY_CMDS,
                int ENTRY_COUNTER, int ic, int dc, char *filename)
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
    
    if (ENTRY_COUNTER > 0)
        entFile = fopen(entFilename, "w+");
    
    if (EXTERNAL_COUNTER > 0)
        extFile = fopen(extFilename, "w+");

    /* Print Code Image to Object file */
    fprintf(objectFile, "\t%d %d\n", (ic - 100), dc);
    for (i = 0; i < CODE_COUNTER; i++)
    {
        fprintf(objectFile, "0%d ", CODE_IMAGE[i]->memoryAddress);
        if (CODE_IMAGE[i]->type == R)
        {
            ramWord = calloc(33, sizeof(char));
            temp = numberToBinary(CODE_IMAGE[i]->opcode, 6);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->rs, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->rt, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->rd, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->funct, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->unused, 6);
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
        else if (CODE_IMAGE[i]->type == I)
        {
            ramWord = calloc(33, sizeof(char));
            temp = numberToBinary(CODE_IMAGE[i]->opcode, 6);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->rs, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->rt, 5);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->immed, 16);
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
        else if (CODE_IMAGE[i]->type == J)
        {
            ramWord = calloc(33, sizeof(char));
            temp = numberToBinary(CODE_IMAGE[i]->opcode, 6);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->reg, 1);
            strcat(ramWord, temp);
            temp = numberToBinary(CODE_IMAGE[i]->reg, 25);
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
    endOfCode = CODE_IMAGE[i - 1]->memoryAddress + 4;
    fprintf(objectFile, "0%d ", endOfCode);

    for (i = 0; i < DATA_COUNTER; i++)
    {
        for (j = 0; j < DATA_IMAGE[i]->size; j++)
        {
            if (DATA_IMAGE[i]->dataRows[j].type == INT)
            {
                if ((counter % 4 == 0) && (counter != 0))
                {
                    fprintf(objectFile, "\n");
                    endOfCode += 4;
                    fprintf(objectFile, "0%d ", endOfCode);
                }

                temp = numberToBinary(DATA_IMAGE[i]->dataRows[j].num, (DATA_IMAGE[i]->byteMulti * 8));

                if (DATA_IMAGE[i]->byteMulti == 1)
                {
                    bin2Hex(temp, objectFile);
                    counter++;
                }
                else if (DATA_IMAGE[i]->byteMulti == 2)
                {
                    b = DATA_IMAGE[i]->byteMulti * 8;
                    for (k = 0; k < DATA_IMAGE[i]->byteMulti; k++)
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
                else if (DATA_IMAGE[i]->byteMulti == 4)
                {
                    b = DATA_IMAGE[i]->byteMulti * 8;
                    for (k = 0; k < DATA_IMAGE[i]->byteMulti; k++)
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
            else if (DATA_IMAGE[i]->dataRows[j].type == STRING)
            {
                if ((counter % 4 == 0) && (counter != 0))
                {
                    fprintf(objectFile, "\n");
                    endOfCode += 4;
                    fprintf(objectFile, "0%d ", endOfCode);
                }
                temp = charToBinary(DATA_IMAGE[i]->dataRows[j].c);
                bin2Hex(temp, objectFile);
                counter++;
            }
        }
    }
    fprintf(objectFile, "\n");
    fclose(objectFile);

    for (i = 0; i < EXTERNAL_COUNTER; i++)
    {

        fprintf(extFile, "%s ", EXTERNAL_CMDS[i]->labelName);
        fprintf(extFile, "0%d\n ", EXTERNAL_CMDS[i]->commandAddress);
    }

    for (i = 0; i < ENTRY_COUNTER; i++)
    {
        fprintf(entFile, "%s ", ENTRY_CMDS[i]->labelName);
        fprintf(entFile, "0%d\n", ENTRY_CMDS[i]->commandAddress);
    }
}

/*
    Print binary string as two hex numbers
*/
void bin2Hex(char *str, FILE *file)
{
    char *a = calloc(9, sizeof(char));
    int b;
    int num = 0;
    strcpy(a, str);
    do
    {
        b = *a == '1' ? 1 : 0;
        num = (num << 1) | b;
        a++;
    } while (*a);

    fprintf(file, "%02X ", num);
}

/*
    Convert integer to binary string
    n - integer to convert
    bytes - numbe of bits 
*/
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

/*
    Convert char to 8 bit binary string
*/
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
    binary[9] = '\0';

    return binary;
}