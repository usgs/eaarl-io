#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        printf("Usage: example_file_tld_opener <tld path> <tld file>\n");
        return 1;
    }

    char const *tld_path = argv[1];
    char const *tld_file = argv[2];
    struct eaarlio_tld_opener tld_opener;
    struct eaarlio_stream stream;
    struct eaarlio_raster raster;
    eaarlio_error err;
    int failed;

    int include_pulses = 0;
    int include_waveforms = 0;

    err = eaarlio_file_tld_opener(&tld_opener, tld_path, NULL);
    failed = eaarlio_error_check(err, "Problem opening tld_opener");
    if(failed)
        return 1;

    err = tld_opener.open_tld(&tld_opener, &stream, tld_file);
    failed = eaarlio_error_check(err, "Problem opening tld");
    if(failed)
        return 1;

    err = eaarlio_tld_read_raster(
        &stream, &raster, NULL, include_pulses, include_waveforms);
    failed = eaarlio_error_check(err, "Problem reading raster");
    if(failed)
        return 1;

    // Fields provided from the raster header
    printf("time_seconds: %" PRIu32 "\n", raster.time_seconds);
    printf("time_fraction: %" PRIu32 "\n", raster.time_fraction);
    printf("sequence_number: %" PRIu32 "\n", raster.sequence_number);
    printf("digitizer: %" PRIu8 "\n", raster.digitizer);
    printf("pulse_count: %" PRIu16 "\n", raster.pulse_count);

    // We don't actually need to call this since raster.pulse is NULL, but it's
    // a good habit to always call it anyway. If no memory is allocated, it's a
    // no-op.
    err = eaarlio_raster_free(&raster, NULL);
    failed = eaarlio_error_check(err, "Problem releasing memory for raster");
    if(failed)
        return 1;

    err = stream.close(&stream);
    failed = eaarlio_error_check(err, "Problem closing tld");
    if(failed)
        return 1;

    err = tld_opener.close(&tld_opener);
    failed = eaarlio_error_check(err, "Problem closing tld_opener");
    if(failed)
        return 1;

    return 0;
}
