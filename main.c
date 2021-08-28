#include "data_structures.h"

int main(int argc, char * argv[])
{

    int i = 1;
    Error * error  = (Error *) calloc(1, sizeof(Error));
    Boolean fileHasErrors = FALSE;

    if (argc < 2) {
        error->code = MISSING_FILENAME;
        printError(*error, &fileHasErrors);
        return 1;
    }

    while (i < argc) {

        error->code = INITIALIZE;
        error->filename = argv[i];
        printError(*error, &fileHasErrors);

        parseFile(argv[i]);


        error->code = FINISH_FILE;
        printError(*error, &fileHasErrors);
        i++;
    }



    return 0;
}