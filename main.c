/*
    MANAN 14 - Assembler project

    Tom Zohar

    This program translate Assembly language into machine code
    it can take multiple .as files and create for each file .ob .ent and .ext files
*/

#include "data_structures.h"

int main(int argc, char * argv[])
{

    int i = 1;
    Error * error  = (Error *) calloc(1, sizeof(Error));

    if (argc < 2) {
        error->code = MISSING_FILENAME;
        printError(*error, NULL);
        return 1;
    }

    while (i < argc) {

        error->code = INITIALIZE;
        error->filename = argv[i];
        printError(*error, NULL);

        parseFile(argv[i]);


        error->code = FINISH_FILE;
        printError(*error, NULL);

        i++;
    }



    return 0;
}