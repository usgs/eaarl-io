#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eaarlio/error.h"

int main(int argc, char *argv[])
{
    int i;
    eaarlio_error err;
    int failed;

    if(argc < 2 || 0 == strncmp(argv[1], "-h", 3) || 0 == strncmp(argv[1], "--help", 7)) {
        printf("Usage: example_errors [-h] <code> [<code> ...]\n");
        printf("Example of interpreting error codes\n");
        printf("\n");
        printf("  -h, --help  display this help and exit\n");
        printf("  <code>      an integer value representing an error code\n");
        return 1;
    }

    for(i = 1; i < argc; i++) {
        printf("---\nTest value %d: %s\n\n", i, argv[i]);

        // In your code, err would be the result of a library function call.
        // For testing purposes, here we just convert a provided int to an enum
        // value.
        err = (eaarlio_error)atoi(argv[i]);

        // The simplest method for error checking is to compare to
        // EAARLIO_SUCCESS
        if(err == EAARLIO_SUCCESS) {
            printf("Success\n\n");
        } else {
            printf("Failure\n\n");
        }

        // You can report readable information on errors
        printf("name: %s\n", eaarlio_error_name(err));
        printf("message: %s\n", eaarlio_error_message(err));
        printf("\n");

        // As an alternative to the above, you can use eaarlio_error_check
        // which returns 0 on success and 1 on failure. On failure, it prints
        // out an error message for you to stderr.
        failed = eaarlio_error_check(err, "entry %d failed", i);
        if(!failed) {
            printf("Success\n");
        }
        printf("\n");
    }

    return 0;
}
