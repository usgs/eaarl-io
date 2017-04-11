#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "argtable3.h"

#include "eaarlio/edb.h"
#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/flight.h"
#include "eaarlio/pulse.h"
#include "eaarlio/raster.h"
#include "eaarlio/units.h"
#include "eaarlio/version.h"

/* Writes out a waveform as a list of integers. To improve readability,the
 * integers are written out using inline list format (i.e., in brackets) rather
 * than block list format (i.e., one value per line). Long lines are wrapped to
 * further improve readability.
 *
 * The output will resemble this:
 *
 *      [
 *          100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
 *          100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
 *          100, 100, 100, 100, 100, 100, 100
 *      ]
 */
void yaml_wf(FILE *out, // Output filehandle
    unsigned char *wf, // Waveform to write
    uint16_t wf_len) // Length of the waveform
{
    // Current index into the wf array
    uint16_t i = 0;

    // Special handling for an empty waveform
    if(!wf || wf_len < 1) {
        fprintf(out, "[]");
        return;
    }

    // The list opens with a bracket
    fprintf(out, "[");

    for(i = 0; i < wf_len; i++) {
        // 12 values per line. Each line is indented.
        if(i % 12 == 0)
            fprintf(out, "\n            ");

        // Write out the current value. If it's not the last, add a comma
        fprintf(out, " %3d", wf[i]);
        if(i + 1 < wf_len)
            fprintf(out, ",");
    }

    // The list closes with an indented bracket
    fprintf(out, "\n          ]\n");
}

/* Write the data for a pulse
 */
void yaml_pulse(FILE *out, // Output filehandle
    struct eaarlio_raster *raster, // The raster's data
    uint16_t pulse_number, // Pulse number to write
    int include_waveforms) // Output waveforms? 1 = yes, 0 = no
{
    // Sanity checks
    assert(out);
    assert(raster);
    assert(raster->pulse);

    // Loop counter
    int i = 0;

    // Load the pulse of interest into a convenience variable
    struct eaarlio_pulse *pulse = &raster->pulse[pulse_number - 1];

    // Number of waveforms, which cannot exceed the max
    int channels = EAARLIO_MAX_RX_COUNT;
    if(pulse->rx_count < channels)
        channels = pulse->rx_count;

    // Each pulse is an entry in a list, so the first line has to get the
    // hyphen prefix.
    fprintf(out, "    - pulse_number: %" PRIu16 "\n", pulse_number);

    // Time and scan angle are stored in hardware-specific formats. We include
    // the values converted into normal units as well.
    fprintf(out, "      time: %.6f\n",
        eaarlio_units_pulse_time(raster, pulse_number));
    fprintf(
        out, "      scan_angle: %.3f\n", eaarlio_units_pulse_scan_angle(pulse));

    // Values directly from the struct
    fprintf(out, "      time_offset: %" PRIu32 "\n", pulse->time_offset);
    fprintf(out, "      scan_angle_counts: %" PRIi16 "\n",
        pulse->scan_angle_counts);
    fprintf(out, "      range: %" PRIu16 "\n", pulse->range);
    fprintf(out, "      rx_count: %" PRIu8 "\n", pulse->rx_count);
    fprintf(out, "      thresh_tx: %" PRIu8 "\n", pulse->thresh_tx);
    fprintf(out, "      thresh_rx: %" PRIu8 "\n", pulse->thresh_rx);
    fprintf(out, "      bias_tx: %" PRIu8 "\n", pulse->bias_tx);

    // The bias_rx field always has four values, regardless of rx_count
    fprintf(out,
        "      bias_rx: [%" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8 "]\n",
        pulse->bias_rx[0], pulse->bias_rx[1], pulse->bias_rx[2],
        pulse->bias_rx[3]);

    // If we don't want waveforms, we're done
    if(!include_waveforms)
        return;

    // Output the transmit waveform
    fprintf(out, "      tx: ");
    yaml_wf(out, pulse->tx, pulse->tx_len);

    // Output the return waveforms, if there are any
    if(!pulse->rx || pulse->rx_count < 1) {
        fprintf(out, "      rx: []\n");
    } else {
        fprintf(out, "      rx:\n");
        for(i = 0; i < channels; i++) {
            fprintf(out, "        - ");
            yaml_wf(out, pulse->rx[i], pulse->rx_len[i]);
        }
    }
}

/* Write the data for a pulse
 */
void yaml_raster(FILE *out, // Output filehandle
    int raster_number, // Raster number for this raster
    struct eaarlio_raster *raster, // The raster number for this raster
    int32_t time_offset, // Time offset from EDB data
    int include_pulses, // Output pulses? 1 = yes, 0 = no
    int include_waveforms) // Output waveforms? 1 = yes, 0 = no
{
    // Sanity
    assert(out);
    assert(raster);

    // Loop variable for pulses
    uint16_t pulse_number;

    // Each raster is an entry in a list, so the first line has to get the
    // hyphen prefix.
    fprintf(out, "- raster_number: %d\n", raster_number);

    // Time is stored in a hardware-specific format. We include the time in
    // normal units as well.
    fprintf(out, "  time: %.6f\n", eaarlio_units_raster_time(raster));

    // Time offset supplied by the EDB file.
    fprintf(out, "  edb_time_offset: %" PRIi32 "\n", time_offset);

    // Values directly from the struct
    fprintf(out, "  time_seconds: %" PRIu32 "\n", raster->time_seconds);
    fprintf(out, "  time_fraction: %" PRIu32 "\n", raster->time_fraction);
    fprintf(out, "  sequence_number: %" PRIu32 "\n", raster->sequence_number);
    fprintf(out, "  digitizer: %" PRIu8 "\n", raster->digitizer);
    fprintf(out, "  pulse_count: %" PRIu16 "\n", raster->pulse_count);

    // If we don't want pulses, we're done
    if(!include_pulses)
        return;

    // Output the pulses, if there are any
    if(!raster->pulse || raster->pulse_count < 1) {
        fprintf(out, "  pulses: []\n");
    } else {
        fprintf(out, "  pulses:\n");
        for(pulse_number = 1; pulse_number <= raster->pulse_count;
            pulse_number++) {
            // Put a blank line before every pulse except the first, for
            // readability
            if(pulse_number > 1)
                fprintf(out, "\n");
            yaml_pulse(out, raster, pulse_number, include_waveforms);
        }
    }
}

/* Precheck requested raster numbers against EDB. Drop bad rasters and return
 * the number of rasters kept.
 *
 * The raster numbers are prechecked to remove invalid or problematic rasters
 * using the following criteria:
 *  - Raster numbers must be between 1 and the maximum raster number from the
 *    EDB
 *  - The EDB record must reference a valid TLD file (i.e, its file index must
 *    be between 1 and the maximum file index from the EDB)
 *
 * If problems are encountered, warnings are written to stderr. The
 * raster_numbers array is updated in-place to remove raster numbers with
 * problems. The number of raster numbers that passed the checks is returned.
 */
int precheck_rasters(struct eaarlio_edb *edb, // EDB data to reference
    int *raster_numbers, // Raster numbers requested
    int raster_numbers_count) // Size of raster_numbers
{
    // Sanity
    assert(edb);
    assert(raster_numbers);

    // Index into raster_numbers for the raster being checked
    int src = 0;

    // Index into raster_numbers where the next valid raster goes
    int dst = 0;

    // Current raster number
    int raster_number;

    // Current file index
    int file_index;

    for(src = 0; src < raster_numbers_count; src++) {
        // Bounds check on raster number
        raster_number = raster_numbers[src];
        if(raster_number < 1) {
            fprintf(stderr, "WARNING: rasters must be >= 1, skipping: %d\n",
                raster_number);
            continue;
        }
        if((unsigned int)raster_number > edb->record_count) {
            fprintf(stderr,
                "WARNING: raster number beyond end of EDB, skipping: %d\n",
                raster_number);
            continue;
        }

        // Bounds check on file_index
        file_index = edb->records[raster_number - 1].file_index;
        if(file_index < 1) {
            fprintf(stderr,
                "WARNING: EDB for raster %d references negative file_index of "
                "%d\n",
                raster_number, file_index);
            continue;
        }
        if((unsigned int)file_index > edb->file_count) {
            fprintf(stderr,
                "WARNING: EDB for raster %d references file_index of %d, max "
                "is %d\n",
                raster_number, file_index, edb->file_count);
            continue;
        }

        // Keep the raster if the checks passed
        raster_numbers[dst] = raster_number;
        dst++;
    }

    // Return the number of rasters kept
    return dst;
}

/* Outputs YAML for the rasters requested. Returns 0 on success, 1 on failure.
 */
int eaarlio_yaml(
    const char *out_file, /* Output YAML file to create; NULL for stdout */
    const char *edb_file, /* EDB file to load for the flight */
    const char *tld_path, /* Path to the TLD files */
    int *raster_numbers, /* Array of raster numbers to read */
    int raster_numbers_count, /* Length of raster_numbers array */
    int include_pulses, /* Output pulse data? 1 = yes, 0 = no */
    int include_waveforms) /* Output waveform data? 1 = yes, 0 = no */
{
    // Sanity
    assert(edb_file);
    assert(raster_numbers);

    // Return value from eaarlio functions
    eaarlio_error err;

    // Have we failed? Not yet.
    int failed = 0;

    // Flight for the EDB
    struct eaarlio_flight flight = eaarlio_flight_empty();

    // Current raster data
    struct eaarlio_raster raster = eaarlio_raster_empty();

    // Time offset applied in the EDB data
    int32_t time_offset = 0;

    // Current raster number
    int raster_number = 0;

    // Loop variable
    int i = 0;

    // Output filehandle; defaults to stdout
    FILE *out = stdout;

    // Open the output file if the user provided one
    if(out_file) {
        out = fopen(out_file, "w");
        if(!out) {
            fprintf(
                stderr, "ERROR: Unable to open output file: %s\n", out_file);
            failed = 1;
            goto exit;
        }
    }

    err = eaarlio_file_flight(&flight, edb_file, tld_path, NULL);
    failed = eaarlio_error_check(err, "ERROR: Problem loading EDB");
    if(failed)
        goto exit;

    // Check for and remove problematic raster numbers
    raster_numbers_count =
        precheck_rasters(&flight.edb, raster_numbers, raster_numbers_count);
    if(raster_numbers_count < 1) {
        fprintf(stderr, "WARNING: no valid rasters were detected\n");
        goto exit;
    }

    // Start the YAML document
    fprintf(out, "---\n");

    for(i = 0; i < raster_numbers_count; i++) {
        // Put a blank line before every raster except the first, for
        // readability
        if(i > 0)
            fprintf(out, "\n");

        raster_number = raster_numbers[i];

        err = eaarlio_flight_read_raster(&flight, &raster, &time_offset,
            raster_number, include_pulses, include_waveforms);
        failed = eaarlio_error_check(
            err, "ERROR: Problem reading raster %d", raster_number);
        if(failed)
            goto exit;

        // Generate the YAML for the raster
        yaml_raster(out, raster_number, &raster, time_offset, include_pulses,
            include_waveforms);

        err = eaarlio_raster_free(&raster, NULL);
        failed = eaarlio_error_check(
            err, "ERROR: Problem releasing memory for raster");
        if(failed)
            goto exit;
    }

    // End the YAML document
    fprintf(out, "...\n");

    // Close the output file (but don't close stdout)
    if(out_file) {
        failed = fclose(out);
        out = NULL;
        if(failed) {
            fprintf(
                stderr, "ERROR: Unable to close output file: %s\n", out_file);
            goto exit;
        }
    }

    err = eaarlio_flight_free(&flight);
    failed = eaarlio_error_check(
        err, "ERROR: Problem releasing resouces for the flight");
    if(failed)
        goto exit;

exit:
    // Cleanup. Make sure files are closed and memory released.
    eaarlio_flight_free(&flight);
    eaarlio_raster_free(&raster, NULL);
    if(out && out_file) {
        fclose(out);
    }

    return failed;
}

/* The main function handles command line arguments. It defines the command
 * line arguments accepted and parses them. It either shows an error, shows
 * usage informatino, or invokes eaarlio_yaml to do the real work.
 */
int main(int argc, char *argv[])
{
    int failed = 0, nerrors = 0;
    char progname[] = "eaarlio_yaml";
    char *tld_path = NULL;
    int tld_path_free = 0;

    struct arg_lit *help, *version, *nopulse, *nowf;
    struct arg_file *outfile, *edb, *tld;
    struct arg_int *rasters;
    struct arg_end *end;

    void *argtable[] = {
        help = arg_litn("h", "help", 0, 1, "display this help and exit"),
        version =
            arg_litn("V", "version", 0, 1, "display library version and exit"),
        outfile = arg_filen("o", "output", "<output.yaml>", 0, 1,
            "output file to create instead of writing to stdout"),
        nopulse = arg_litn(
            "P", "no-pulses", 0, 1, "do not include pulse data in the output"),
        nowf = arg_litn("W", "no-waveforms", 0, 1,
            "do not include waveform data in the output"),
        tld = arg_file0("t", "tld", "<tld path>", "path to the TLD files"),
        edb = arg_filen(NULL, NULL, "<edb file>", 1, 1, "EDB file for dataset"),
        rasters = arg_intn(
            NULL, NULL, "<raster number>", 1, 1000, "raster numbers to export"),
        end = arg_end(20),
    };

    if(arg_nullcheck(argtable) != 0) {
        printf("error: insufficient memory\n");
        failed = 1;
        goto exit;
    }

    // Output filename defaults to NULL for stdout
    outfile->filename[0] = NULL;

    // Parse the command line
    nerrors = arg_parse(argc, argv, argtable);

    // Display version number if --version is requested
    if(version->count > 0) {
        printf("%s\n", EAARLIO_VERSION);
        failed = 0;
        goto exit;
    }

    // Show usage information if --help is requested
    if(help->count > 0) {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Export rasters in YAML format.\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        printf(
            "\n"
            "Exports the rasters in YAML format as requested...\n");
        failed = 0;
        goto exit;
    }

    // Display errors if invalid args were given
    if(nerrors > 0) {
        arg_print_errors(stderr, end, progname);
        fprintf(stderr, "Try '%s --help' for more information.\n", progname);
        failed = 1;
        goto exit;
    }

    assert(strlen(edb->filename[0]) > 0);

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

    // Pass the parsed args to @ref eaarlio_yaml to do the real work
    failed = eaarlio_yaml(outfile->filename[0], edb->filename[0], tld_path,
        rasters->ival, rasters->count, nopulse->count == 0, nowf->count == 0);

exit:
    // Cleanup: release memory
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    if(tld_path_free && tld_path)
        free(tld_path);

    return failed;
}
