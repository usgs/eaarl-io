#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "eaarlio/edb.h"
#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/units.h"

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Usage: example_edb_read <file.edb> [<record_number> ...]\n");
        return 1;
    }

    char const *edb_file = argv[1];

    int i;
    int failed;
    eaarlio_error err;
    uint32_t record_number;
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_edb edb = eaarlio_edb_empty();
    struct eaarlio_edb_record record = eaarlio_edb_record_empty();
    char const *tld_file = NULL;

    // For this example, we want to load the records and files from the EDB.
    int include_records = 1;
    int include_files = 1;

    // Open the file using a file-based stream
    err = eaarlio_file_stream(&stream, edb_file, "r");
    failed = eaarlio_error_check(err, "Problem opening EDB");
    if(failed)
        return 1;

    // Read the EDB from the stream
    err = eaarlio_edb_read(&stream, &edb, NULL, include_records, include_files);
    failed = eaarlio_error_check(err, "Problem reading EDB");
    if(failed)
        return 1;

    // Close the stream
    err = stream.close(&stream);
    failed = eaarlio_error_check(err, "Problem closing EDB");
    if(failed)
        return 1;

    // Fields provided by edb
    printf("record_count: %" PRIu32 "\n", edb.record_count);
    printf("file_count: %" PRIu32 "\n", edb.file_count);

    for(i = 2; i < argc; i++) {
        // Parse the record number provided
        record_number = (uint32_t)atoi(argv[i]);
        printf("\nRecord %" PRIu32 "\n", record_number);

        // Record numbers are 1-based indexes into edb.records
        if(record_number < 1 || record_number > edb.record_count) {
            printf("Out of bounds, skipping\n");
            continue;
        }
        record = edb.records[record_number - 1];

        // Fields provided by the record
        printf("time_seconds: %" PRIu32 "\n", record.time_seconds);
        printf("time_fraction: %" PRIu32 "\n", record.time_fraction);
        printf("record_offset: %" PRIu32 "\n", record.record_offset);
        printf("record_length: %" PRIu32 "\n", record.record_length);
        printf("file_index: %" PRIi16 "\n", record.file_index);
        printf("pulse_count: %" PRIu8 "\n", record.pulse_count);
        printf("digitizer: %" PRIu8 "\n", record.digitizer);

        // The time_seconds and time_fraction fields encode the time in a
        // hardware specific manner. This is how you decode the time.
        printf("time: %f\n", eaarlio_units_edb_time(&record));

        // The file_index is a 1-based index into edb.files for the
        // corresponding TLD file
        if(record.file_index < 1
            || (uint32_t)record.file_index > edb.file_count) {
            printf("file_index is out of bounds\n");
            continue;
        }
        tld_file = edb.files[record.file_index - 1];
        printf("tld file: %s\n", tld_file);
    }

    // Release resources
    err = eaarlio_edb_free(&edb, NULL);
    failed =
        eaarlio_error_check(err, "Problem releasing resources for the EDB");
    if(failed)
        return 1;

    return 0;
}
