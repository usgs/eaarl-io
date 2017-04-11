#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/flight.h"
#include "eaarlio/raster.h"
#include "eaarlio/units.h"

int main(int argc, char *argv[])
{
    if(argc < 3) {
        printf(
            "Usage: example_flight_read_rasters <file.edb> <raster_number> "
            "[...]\n");
        return 1;
    }

    char const *edb_file = argv[1];
    struct eaarlio_flight flight = eaarlio_flight_empty();
    struct eaarlio_raster raster = eaarlio_raster_empty();
    int32_t time_offset = 0;
    eaarlio_error err;
    int failed;
    int i;
    int raster_number;

    // For this example, we'll skip loading pulses and waveforms data. This
    // means only the raster header is loaded.
    int include_pulses = 0;
    int include_waveforms = 0;

    // Open the flight. This reads the EDB and initializes the flight so that
    // it's ready to read rasters.
    err = eaarlio_file_flight(&flight, edb_file, ".", NULL);
    failed = eaarlio_error_check(err, "Problem loading EDB");
    if(failed)
        return 1;

    // Iterate over the raster numbers requested. Note that they do not need to
    // be sequential or monotonic.
    for(i = 2; i < argc; i++) {
        raster_number = atoi(argv[i]);
        printf("raster_number: %d\n", raster_number);

        // Read a raster using the flight.
        err = eaarlio_flight_read_raster(&flight, &raster, &time_offset,
            raster_number, include_pulses, include_waveforms);
        failed = eaarlio_error_check(err, "Problem reading raster");
        if(failed)
            return 1;

        // Fields provided from the raster header
        printf("time_seconds: %" PRIu32 "\n", raster.time_seconds);
        printf("time_fraction: %" PRIu32 "\n", raster.time_fraction);
        printf("sequence_number: %" PRIu32 "\n", raster.sequence_number);
        printf("digitizer: %" PRIu8 "\n", raster.digitizer);
        printf("pulse_count: %" PRIu16 "\n", raster.pulse_count);

        // raster.pulse was not populated, since we specified include_pulses = 0

        // The time_offset is the difference in time between the timestamp in
        // the EDB record and the timestamp in the TLD file. If this isn't 0,
        // then it means a correction was applied to the EDB.
        printf("edb_time_offset: %" PRIi32 "\n", time_offset);

        // The time_seconds and time_fraction fields encode the time in a
        // hardware specific manner. This is how you decode the time.
        printf("time: %.6f\n", eaarlio_units_raster_time(&raster));

        // We don't actually need to call this since raster.pulse is NULL, but
        // it's  a good habit to always call it anyway. If no memory is
        // allocated, it's a no-op.
        err = eaarlio_raster_free(&raster, NULL);
        failed =
            eaarlio_error_check(err, "Problem releasing memory for raster");
        if(failed)
            return 1;

        printf("\n");
    }

    // Close the flight and release resources.
    err = eaarlio_flight_free(&flight);
    failed =
        eaarlio_error_check(err, "Problem releasing resouces for the flight");
    if(failed)
        return 1;

    return 0;
}
