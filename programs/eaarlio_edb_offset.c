#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "argtable3.h"

#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/flight.h"
#include "eaarlio/raster.h"
#include "eaarlio/version.h"

int do_check(char const *edb_file, char const *tld_path)
{
    eaarlio_error err;
    int failed = 0;
    struct eaarlio_flight flight = eaarlio_flight_empty();
    struct eaarlio_raster raster = eaarlio_raster_empty();
    int32_t time_offset = 0;
    int32_t time_offset_last = 0;
    unsigned int i = 0;
    unsigned int changes = 0;
    int printed_first = 0;

    err = eaarlio_file_flight(&flight, edb_file, tld_path, NULL);
    failed = eaarlio_error_check(err, "ERROR: Problem loading EDB");
    if(failed)
        goto exit;

    for(i = 0; i < flight.edb.record_count; i++) {
        err = eaarlio_flight_read_raster(
            &flight, &raster, &time_offset, i + 1, 0, 0);
        failed =
            eaarlio_error_check(err, "ERROR: Problem reading raster %d", i + 1);
        if(failed)
            goto exit;

        if(time_offset != time_offset_last) {
            if(!printed_first) {
                if(i > 0)
                    printf("Raster 1: offset 0\n");
                printed_first = 1;
            }
            printf("Raster %d: offset %d\n", i + 1, time_offset);
            time_offset_last = time_offset;
            if(i > 0)
                changes++;
        }
    }

    printf("%d rasters scanned. ", flight.edb.record_count);
    if(!changes) {
        if(time_offset == 0) {
            printf("No time offset was detected.\n");
        } else {
            printf("All rasters detected to have the same time offset.\n");
        }
    } else {
        printf("Detected multiple time offsets.\n");
    }

    err = eaarlio_flight_free(&flight);
    failed = eaarlio_error_check(
        err, "ERROR: Problem releasing resources for the flight");
    if(failed)
        goto exit;

exit:
    eaarlio_flight_free(&flight);
    return failed;
}

int do_set(char const *edb_file,
    char const *tld_path,
    int offset,
    int start,
    int stop)
{
    eaarlio_error err;
    int failed = 0;
    struct eaarlio_flight flight = eaarlio_flight_empty();
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_raster raster = eaarlio_raster_empty();
    unsigned int i = 0;
    unsigned int istart = 0;
    unsigned int istop = 0;

    err = eaarlio_file_flight(&flight, edb_file, tld_path, NULL);
    failed = eaarlio_error_check(err, "ERROR: Problem loading EDB");
    if(failed)
        goto exit;

    if(start > 0) {
        istart = (unsigned int)start - 1;
    }
    if(stop > 0) {
        istop = (unsigned int)stop;
    } else {
        istop = flight.edb.record_count;
    }

    for(i = istart; i < istop; i++) {
        err = eaarlio_flight_read_raster(&flight, &raster, NULL, i + 1, 0, 0);
        failed =
            eaarlio_error_check(err, "ERROR: Problem reading raster %d", i + 1);
        if(failed)
            goto exit;

        flight.edb.records[i].time_seconds = raster.time_seconds + offset;
    }

    err = eaarlio_file_stream(&stream, edb_file, "w");
    failed = eaarlio_error_check(
        err, "ERROR: Unable to open %s for writing", edb_file);
    if(failed)
        goto exit;

    err = eaarlio_edb_write(&stream, &flight.edb);
    failed = eaarlio_error_check(err, "ERROR: Problem writing EDB");
    if(failed)
        goto exit;

    err = stream.close(&stream);
    failed = eaarlio_error_check(err, "ERROR: Problem closing %s", edb_file);
    if(failed)
        goto exit;

    err = eaarlio_flight_free(&flight);
    failed = eaarlio_error_check(
        err, "ERROR: Problem releasing resources for the flight");
    if(failed)
        goto exit;

exit:
    eaarlio_flight_free(&flight);
    if(stream.close)
        stream.close(&stream);
    return failed;
}

int do_adjust(char const *edb_file, int adjustment, int start, int stop)
{
    eaarlio_error err;
    int failed = 0;
    struct eaarlio_edb edb = eaarlio_edb_empty();
    struct eaarlio_stream stream = eaarlio_stream_empty();
    unsigned int i = 0;
    unsigned int istart = 0;
    unsigned int istop = 0;

    err = eaarlio_file_stream(&stream, edb_file, "r");
    failed = eaarlio_error_check(err, "ERROR: Problem opening EDB to read");
    if(failed)
        goto exit;

    err = eaarlio_edb_read(&stream, &edb, NULL, 1, 1);
    failed = eaarlio_error_check(err, "ERROR: Problem reading EDB");
    if(failed)
        goto exit;

    err = stream.close(&stream);
    failed = eaarlio_error_check(err, "ERROR: Problem closing EDB after read");
    if(failed)
        goto exit;

    if(start > 0) {
        istart = (unsigned int)start - 1;
    }
    if(stop > 0) {
        istop = (unsigned int)stop;
    } else {
        istop = edb.record_count;
    }

    for(i = istart; i < istop; i++) {
        edb.records[i].time_seconds += adjustment;
    }

    err = eaarlio_file_stream(&stream, edb_file, "w");
    failed = eaarlio_error_check(err, "ERROR: Problem opening EDB to write");
    if(failed)
        goto exit;

    err = eaarlio_edb_write(&stream, &edb);
    failed = eaarlio_error_check(err, "ERROR: Problem writing EDB");
    if(failed)
        goto exit;

    err = stream.close(&stream);
    failed = eaarlio_error_check(err, "ERROR: Problem closing EDB after write");
    if(failed)
        goto exit;

    err = eaarlio_edb_free(&edb, NULL);
    failed = eaarlio_error_check(
        err, "ERROR: Problem releasing resources for the EDB");
    if(failed)
        goto exit;

exit:
    eaarlio_edb_free(&edb, NULL);
    if(stream.close)
        stream.close(&stream);
    return failed;
}

int main(int argc, char *argv[])
{
    int failed = 0, nerrors = 0;
    char progname[] = "eaarlio_edb_offset";
    char *tld_path = NULL;
    int tld_path_free = 0;

    struct arg_lit *help, *version;
    struct arg_int *adjust, *set, *start, *stop;
    struct arg_file *edb, *tld;
    struct arg_end *end;

    void *argtable[] = {
        help = arg_lit0("h", "help", "display this help and exit"),
        version = arg_lit0("V", "version", "display library version and exist"),
        adjust = arg_int0("a", "adjust", "<amount>",
            "offset to apply (modifies current offfset)"),
        set = arg_int0("s", "set", "<offset>",
            "offset to assign (replaces current offset)"),
        start = arg_int0(NULL, "start", "<raster number>",
            "raster number to start at (for --set and --adjust)"),
        stop = arg_int0(NULL, "stop", "<raster number>",
            "raster number to stop at (for --set and --adjust)"),
        tld = arg_file0("t", "tld", "<tld path>", "path to the TLD files"),
        edb =
            arg_file1(NULL, NULL, "<edb file>", "EDB file to check or modify"),
        end = arg_end(20),
    };

    if(arg_nullcheck(argtable) != 0) {
        printf("error: insufficient memory\n");
        failed = 1;
        goto exit;
    }

    start->ival[0] = 0;
    stop->ival[0] = 0;

    nerrors = arg_parse(argc, argv, argtable);

    if(version->count > 0) {
        printf("%s\n", EAARLIO_VERSION);
        failed = 0;
        goto exit;
    }

    if(help->count > 0) {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Work with time offsets.\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        failed = 0;
        goto exit;
    }

    if(nerrors > 0) {
        arg_print_errors(stdout, end, progname);
        printf("Try '%s --help' for more information.\n", progname);
        failed = 1;
        goto exit;
    }

    if(set->count > 0 || adjust->count == 0) {
        if(tld->count > 0) {
            tld_path = (char *)tld->filename[0];
        } else if(strlen(edb->basename[0]) < strlen(edb->filename[0])) {
            tld_path_free = 1;
            tld_path = calloc(strlen(edb->filename[0] + 1), sizeof(char));
            if(!tld_path) {
                fprintf(stderr, "Memory allocation failure\n");
                failed = 1;
                goto exit;
            }
            strcpy(tld_path, edb->filename[0]);
            tld_path[strlen(edb->filename[0]) - strlen(edb->basename[0])] = 0;
        } else {
            tld_path = ".";
        }
    }

    if(set->count > 0) {
        failed = do_set(edb->filename[0], tld_path, set->ival[0],
            start->ival[0], stop->ival[0]);
    } else if(adjust->count > 0) {
        failed = do_adjust(
            edb->filename[0], adjust->ival[0], start->ival[0], stop->ival[0]);
    } else {
        failed = do_check(edb->filename[0], tld_path);
    }

exit:
    if(tld_path_free)
        free(tld_path);
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return failed;
}
