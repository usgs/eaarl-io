#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "argtable3.h"

#include "eaarlio/edb.h"
#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/raster.h"
#include "eaarlio/stream.h"
#include "eaarlio/tld.h"
#include "eaarlio/version.h"

/**
 * Check if memory allocation succeeded
 *
 * @param[in] ptr Pointer for memory that was just allocated
 *
 * @returns 0 (success) if @p ptr is not @c NULL
 * @returns 1 (failure) if @p ptr is @c NULL
 *
 * @post On failure, prints an error message to stderr.
 *
 * @remark The error message will look like this:
 *
 *      ERROR: unable to allocate memory
 */
int check_mem(void *ptr)
{
    if(ptr)
        return 0;
    fprintf(stderr, "ERROR: Unable to allocate memory\n");
    return 1;
}

int eaarlio_edb_create(const char *outfile,
    const char **infiles,
    const char **infiles_base,
    int infiles_count,
    uint32_t records_size,
    int verbose)
{
    eaarlio_error err = EAARLIO_SUCCESS;
    int exitcode = 0, statuscode = 0;

    /* Offset in TLD file to start of current record */
    int64_t record_offset = 0;
    /* Current record's header */
    struct eaarlio_tld_header record_header;
    /* Current record's raster */
    struct eaarlio_raster raster;
    /* Count of records in current TLD */
    uint32_t tld_record_count = 0;

    struct eaarlio_edb edb = {.records = NULL, .files = NULL };
    struct eaarlio_edb_record *records = NULL;
    uint32_t record_index = 0;
    int16_t file_index = 0;

    struct eaarlio_stream stream = eaarlio_stream_empty();

    if(records_size < 1)
        records_size = 1024 * infiles_count;
    records = calloc(records_size, sizeof(struct eaarlio_edb_record));
    exitcode = check_mem(records);
    if(exitcode)
        goto exit;
    if(verbose > 2) {
        printf("Initial records array size is %d\n", records_size);
    }

    if(verbose > 0) {
        if(verbose > 1)
            printf("\n");
        printf("Scanning %d input TLD files:\n", infiles_count);
    }

    record_index = 0;
    for(file_index = 1; file_index <= infiles_count; file_index++) {
        if(verbose > 0) {
            printf("[%3d/%3d]: %s\n", file_index, infiles_count,
                infiles[file_index - 1]);
        }

        err = eaarlio_file_stream(&stream, infiles[file_index - 1], "r");
        exitcode = eaarlio_error_check(
            err, "ERROR: Unable to open %s", infiles[file_index - 1]);
        if(exitcode)
            goto exit;

        tld_record_count = 0;
        while(1) {
            if(record_index == records_size) {
                records_size *= 2;
                records = realloc(
                    records, records_size * sizeof(struct eaarlio_edb_record));
                exitcode = check_mem(records);
                if(exitcode)
                    goto exit;
                if(verbose > 2) {
                    printf(
                        "  Increased records array size to %d\n", records_size);
                }
            }

            err = stream.tell(&stream, &record_offset);
            statuscode = eaarlio_error_check(err,
                "WARNING: Tell failed on %s, continuing to next file",
                infiles[file_index - 1]);
            if(statuscode)
                break;
            if(record_offset < 0 || record_offset > UINT32_MAX) {
                fprintf(stderr,
                    "WARNING: record offset %" PRIi64
                    " in file %s exceeds "
                    "bounds of EDB record storage, continuing to next file\n",
                    record_offset, infiles[file_index - 1]);
                break;
            }

            err = eaarlio_tld_read_record(
                &stream, &record_header, &raster, NULL, 0, 0);
            if(err == EAARLIO_STREAM_READ_SHORT)
                break;
            statuscode = eaarlio_error_check(err,
                "WARNING: Encountered issue reading %s at offset %" PRIi64
                ", "
                "continuing to next file\n",
                infiles[file_index - 1], record_offset);
            if(statuscode)
                break;
            if(record_header.record_type != EAARLIO_TLD_TYPE_RASTER) {
                if(verbose > 0) {
                    printf("  Skipping record with type %d at offset %lld\n",
                        record_header.record_type, (long long)record_offset);
                }
                continue;
            }

            if(raster.pulse_count > UINT8_MAX) {
                fprintf(stderr,
                    "WARNING: Encountered pulse count out of range for EDB "
                    "in %s at offset %" PRIi64 "\n",
                    infiles[file_index - 1], record_offset);
                raster.pulse_count = UINT8_MAX;
            }

            records[record_index].time_seconds = raster.time_seconds;
            records[record_index].time_fraction = raster.time_fraction;
            records[record_index].record_offset = (uint32_t)record_offset;
            records[record_index].record_length = record_header.record_length;
            records[record_index].file_index = file_index;
            records[record_index].pulse_count = (uint8_t)raster.pulse_count;
            records[record_index].digitizer = raster.digitizer;

            record_index++;
            tld_record_count++;
        }
        if(verbose > 1) {
            printf("  Found %d records, total is %d\n", tld_record_count,
                record_index);
        }

        err = stream.close(&stream);
        exitcode = eaarlio_error_check(
            err, "ERROR: Problem closing %s", infiles[file_index - 1]);
        if(exitcode)
            goto exit;
    }

    err = eaarlio_file_stream(&stream, outfile, "w");
    exitcode = eaarlio_error_check(err, "ERROR: Unable to open %s", outfile);
    if(exitcode)
        goto exit;

    edb.record_count = record_index;
    edb.records = records;
    edb.file_count = infiles_count;
    edb.files = (char **)infiles_base;
    err = eaarlio_edb_write(&stream, &edb);
    exitcode = eaarlio_error_check(err, "ERROR: Problem during EDB creation");
    if(exitcode)
        goto exit;

    err = stream.close(&stream);
    exitcode = eaarlio_error_check(err, "ERROR: Problem closing %s", outfile);
    if(exitcode)
        goto exit;

    if(verbose > 0) {
        printf("\n");
        printf("Created %s with %d records\n", outfile, record_index);
    }

exit:
    if(stream.close)
        stream.close(&stream);
    if(records)
        free(records);

    return exitcode;
}

int main(int argc, char *argv[])
{
    int exitcode = 0, nerrors = 0;
    char progname[] = "eaarlio_edb_create";

    struct arg_lit *help, *version, *verbose;
    struct arg_int *records;
    struct arg_file *outfile, *infiles;
    struct arg_end *end;

    void *argtable[] = {
        help = arg_litn("h", "help", 0, 1, "display this help and exit"),
        version =
            arg_litn("V", "version", 0, 1, "display library version and exit"),
        verbose = arg_litn("v", "verbose", 0, 3,
            "display more information while running, repeat up"),
        arg_rem(NULL, "to 3 times for more"),
        records = arg_int0(NULL, "record-count", NULL,
            "hint on how many records are expected, default is"),
        arg_rem(NULL, "1024 * tld file count"),
        outfile = arg_filen("o", "output", "<edb file>", 0, 1,
            "EDB file to create, default is eaarl.idx"),
        infiles = arg_filen(
            NULL, NULL, "<tld file>", 1, 1000, "TLD files to process"),
        end = arg_end(20),
    };

    if(arg_nullcheck(argtable) != 0) {
        printf("error: insufficient memory\n");
        exitcode = 1;
        goto exit;
    }

    /* Defaults */
    outfile->filename[0] = "eaarl.idx";
    records->ival[0] = 0;

    nerrors = arg_parse(argc, argv, argtable);

    if(version->count > 0) {
        printf("%s\n", EAARLIO_VERSION);
        exitcode = 0;
        goto exit;
    }

    if(help->count > 0 || infiles->count == 0) {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Create an EAARL EDB file.\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        printf(
            "\n"
            "Given a list of EAARL-A or EAARL-B TLD lidar data files, this "
            "program\n"
            "generates a corresponding EDB index file. The TLD files will be "
            "processed in\n"
            "the order given, so they should be provided in order by time.\n"
            "\n"
            "The output file is eaarl.idx (in the current directory) by "
            "default, but that\n"
            "can be changed with the --output option.\n"
            "\n"
            "The --record-count options is generally not needed but can be "
            "used to\n"
            "optimize how the program initially allocates memory.\n");

        exitcode = 0;
        goto exit;
    }

    if(nerrors > 0) {
        arg_print_errors(stdout, end, progname);
        printf("Try '%s --help' for more information.\n", progname);
        exitcode = 1;
        goto exit;
    }

    exitcode = eaarlio_edb_create(outfile->filename[0], infiles->filename,
        infiles->basename, infiles->count, records->ival[0], verbose->count);

exit:
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
}
