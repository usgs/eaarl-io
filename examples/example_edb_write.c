#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eaarlio/edb.h"
#include "eaarlio/error.h"
#include "eaarlio/file.h"

int main(int argc, char *argv[])
{
    if(argc != 2 || 0 == strncmp(argv[1], "-h", 3) || 0 == strncmp(argv[1], "--help", 7)) {
        printf("Usage: example_edb_write [-h] <edb file>\n");
        printf("Example of writing to an EDB, using fixed sample data\n");
        printf("\n");
        printf("  -h, --help  display this help and exit\n");
        printf("  <edb file>  EDB file to write\n");
        return 1;
    }

    char const *edb_file = argv[1];

    int failed;
    eaarlio_error err;
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_edb edb = eaarlio_edb_empty();

    // For the purposes of this example, we're writing out an EDB using
    // hard-coded data.

    struct eaarlio_edb_record records[] = {
        {
            .time_seconds = 1383685470,
            .time_fraction = 191852,
            .record_offset = 0,
            .record_length = 123718,
            .file_index = 1,
            .pulse_count = 119,
            .digitizer = 0,
        },
        {
            .time_seconds = 1383685470,
            .time_fraction = 223381,
            .record_offset = 123718,
            .record_length = 123674,
            .file_index = 1,
            .pulse_count = 119,
            .digitizer = 1,
        },
        {
            .time_seconds = 1383685470,
            .time_fraction = 253827,
            .record_offset = 0,
            .record_length = 123654,
            .file_index = 2,
            .pulse_count = 119,
            .digitizer = 0,
        },
    };

    char const *files[] = {
        "131105-210430.tld", "131105-210513.tld",
    };

    edb.record_count = 3;
    edb.file_count = 2;
    edb.records = (struct eaarlio_edb_record *)&records;
    edb.files = (char **)&files;

    // Open the file using a file-based stream
    err = eaarlio_file_stream(&stream, edb_file, "w");
    failed = eaarlio_error_check(err, "Problem opening EDB");
    if(failed)
        return 1;

    // Write the EDB to the stream
    err = eaarlio_edb_write(&stream, &edb);
    failed = eaarlio_error_check(err, "Problem writing EDB");
    if(failed)
        return 1;

    // Close the stream
    err = stream.close(&stream);
    failed = eaarlio_error_check(err, "Problem closing EDB");
    if(failed)
        return 1;

    return 0;
}
