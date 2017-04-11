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
    if(argc != 4) {
        printf(
            "Usage: example_flight_read_raster <file.edb> <raster_number> "
            "<pulse_number>\n");
        return 1;
    }

    char const *edb_file = argv[1];
    int raster_number = atoi(argv[2]);
    uint16_t pulse_number = (uint16_t)atoi(argv[3]);

    struct eaarlio_flight flight = eaarlio_flight_empty();
    struct eaarlio_raster raster = eaarlio_raster_empty();
    struct eaarlio_pulse *pulse = NULL;
    int32_t time_offset = 0;
    eaarlio_error err;
    int failed;
    int i, j;
    int channels;

    // For this example, we want to load the pulse and waveform data so that it
    // can be displayed.
    int include_pulses = 1;
    int include_waveforms = 1;

    // Open the flight. This reads the EDB and initializes the flight so that
    // it's ready to read rasters.
    err = eaarlio_file_flight(&flight, edb_file, ".", NULL);
    failed = eaarlio_error_check(err, "Problem loading EDB");
    if(failed)
        return 1;

    // Read a raster using the flight.
    err = eaarlio_flight_read_raster(&flight, &raster, &time_offset,
        raster_number, include_pulses, include_waveforms);
    failed = eaarlio_error_check(err, "Problem reading raster");
    if(failed)
        return 1;

    printf("raster_number: %d\n", raster_number);

    // Fields provided from the raster header
    printf("time_seconds: %" PRIu32 "\n", raster.time_seconds);
    printf("time_fraction: %" PRIu32 "\n", raster.time_fraction);
    printf("sequence_number: %" PRIu32 "\n", raster.sequence_number);
    printf("digitizer: %" PRIu8 "\n", raster.digitizer);
    printf("pulse_count: %" PRIu16 "\n", raster.pulse_count);

    // The time_offset is the difference in time between the timestamp in the
    // EDB record and the timestamp in the TLD file. If this isn't 0, then it
    // means a correction was applied to the EDB.
    printf("edb_time_offset: %" PRIi32 "\n", time_offset);

    // The time_seconds and time_fraction fields encode the time in a hardware
    // specific manner. This is how you decode the time.
    printf("time: %.6f\n", eaarlio_units_raster_time(&raster));

    // Make sure the pulse number given is valid
    if(pulse_number < 1 || pulse_number > raster.pulse_count) {
        printf("Requested pulse number %d is out of range\n", pulse_number);
        return 1;
    }

    printf("\n");
    printf("pulse_number: %" PRIu16 "\n", pulse_number);

    // Pulse numbers are 1-based index into the pulse array
    pulse = &raster.pulse[pulse_number - 1];

    // Fields provided by the pulse header
    printf("time_offset: %" PRIu32 "\n", pulse->time_offset);
    printf("scan_angle_counts: %" PRIi16 "\n", pulse->scan_angle_counts);
    printf("range: %" PRIu16 "\n", pulse->range);
    printf("rx_count: %" PRIu8 "\n", pulse->rx_count);
    printf("thresh_tx: %" PRIu8 "\n", pulse->thresh_tx);
    printf("thresh_rx: %" PRIu8 "\n", pulse->thresh_rx);
    printf("bias_tx: %" PRIu8 "\n", pulse->bias_tx);

    // The bias_rx field always has four values, regardless of rx_count
    printf("bias_rx: %" PRIu8 " %" PRIu8 " %" PRIu8 " %" PRIu8 "\n",
        pulse->bias_rx[0], pulse->bias_rx[1], pulse->bias_rx[2],
        pulse->bias_rx[3]);

    // The time_offset field needs to be combined with the raster's time fields
    // to derive the pulse's timestamp.
    printf("time: %.6f\n", eaarlio_units_pulse_time(&raster, pulse_number));

    // The scan angle is stored in a hardware specific manner. This is how to
    // convert it to degrees.
    printf("scan_angle: %.3f\n", eaarlio_units_pulse_scan_angle(pulse));

    printf("\n");

    // Display the transmit waveform
    printf("tx:");
    for(i = 0; i < pulse->tx_len; i++) {
        printf(" %d", pulse->tx[i]);
    }
    printf("\n");

    // The file format and library only support at most EAARLIO_MAX_RX_COUNT
    // return waveforms. You shouldn't ever encounter a record with more than
    // that, but it's a good precaution to check in case of data corruption.
    channels = EAARLIO_MAX_RX_COUNT;
    if(pulse->rx_count < channels)
        channels = pulse->rx_count;

    // Channel numbers are 1-based indexes into the rx array
    for(j = 0; j < channels; j++) {
        printf("rx, channel %d:", j + 1);
        for(i = 0; i < pulse->rx_len[j]; i++) {
            printf(" %d", pulse->rx[j][i]);
        }
        printf("\n");
    }

    pulse = NULL;

    // Release memory allocated within the raster structure.
    err = eaarlio_raster_free(&raster, NULL);
    failed = eaarlio_error_check(err, "Problem releasing memory for raster");
    if(failed)
        return 1;

    // Close the flight and release resources.
    err = eaarlio_flight_free(&flight);
    failed =
        eaarlio_error_check(err, "Problem releasing resouces for the flight");
    if(failed)
        return 1;

    return 0;
}
