#include "error_handler.h"

int main(int argc, char * argv[])
{

    Error * error  = (Error *) calloc(1, sizeof(Error));
    Boolean fileHasErrors = FALSE;

    if (argc < 2) {
        error->code = MISSING_FILENAME;
        printError(*error, &fileHasErrors);
    }

    if (fileHasErrors) {
        printf("File has errors\n");
    }

    while (*++argv) {
        error->code = INITIALIZE;
        error->filename = *argv;
        printError(*error, &fileHasErrors);
    }






    return 0;
}