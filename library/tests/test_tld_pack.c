#include "eaarlio/memory.h"
#include "eaarlio/tld.h"
#include "eaarlio/tld_pack.h"
#include "greatest.h"
#include "assert_error.h"
#include <stdint.h>
#include <stdio.h>

#ifdef _MSC_VER
/* Visual Studio complains about initializing structs with references to
 * non-constant values because it's non-standard, but it's supported on all
 * current target platforms */
#pragma warning(disable : 4204)
/* Visual Studio complains about initializing structs with references to local
 * variables because it's non-standard, but it's supported on all current
 * target platforms */
#pragma warning(disable : 4221)
#endif

/*******************************************************************************
 * eaarlio_tld_pack_raster
 *******************************************************************************
 */

TEST test_raster_sanity()
{
    eaarlio_tld_pack_raster(NULL, NULL, NULL, NULL);
    PASS();
}

TEST test_raster_null_buffer()
{
    uint32_t buffer_len;
    struct eaarlio_raster raster;
    struct eaarlio_memory memory;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_pack_raster(NULL, &buffer_len, &raster, &memory));
    PASS();
}

TEST test_raster_null_buffer_len()
{
    unsigned char *buffer;
    struct eaarlio_raster raster;
    struct eaarlio_memory memory;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_pack_raster(&buffer, NULL, &raster, &memory));
    PASS();
}

TEST test_raster_null_raster()
{
    unsigned char *buffer;
    uint32_t buffer_len;
    struct eaarlio_memory memory;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_pack_raster(&buffer, &buffer_len, NULL, &memory));
    PASS();
}

/* Can we decode values?
 * Do we properly handle multiple pulses?
 * Do we properly handle if the buffer is longer than needed?
 */
TEST test_raster_values(void *arg)
{
    unsigned char **got = (unsigned char **)arg;
    uint32_t got_len;
    unsigned char const exp[] = {
        /* raster_header */
        /* seconds */
        '\x01', '\x02', '\x03', '\x04',
        /* fractional seconds */
        '\x00', '\x00', '\x00', '\x00',
        /* raster number */
        '\x00', '\x00', '\x00', '\x00',
        /* bitfield: pulse count and digitizer */
        '\x02', '\x00',

        /* pulse[0] */
        /* Offset time */
        '\x11', '\x12', '\x13',
        /* Number of waveforms */
        '\x01',
        /* Transmit bias */
        '\x00',
        /* Return biases */
        '\x00', '\x00', '\x00', '\x00',
        /* Scan angle counts */
        '\x00', '\x00',
        /* Bitfield: integer range, threshold flags */
        '\x00', '\x00',
        /* Data length */
        '\x08', '\x00',
        /* tx length */
        '\x02',
        /* tx waveform */
        '\x30', '\x31',
        /* rx[0] length */
        '\x03', '\x00',
        /* rx[0] waveform */
        '\x40', '\x41', '\x42',

        /* pulse[1] */
        /* Offset time */
        '\x21', '\x22', '\x23',
        /* Number of waveforms */
        '\x02',
        /* Transmit bias */
        '\x00',
        /* Return biases */
        '\x00', '\x00', '\x00', '\x00',
        /* Scan angle counts */
        '\x00', '\x00',
        /* Bitfield: integer range, threshold flags */
        '\x00', '\x00',
        /* Data length */
        '\x0B', '\x00',
        /* tx length */
        '\x02',
        /* tx waveform */
        '\x51', '\x52',
        /* rx[0] length */
        '\x02', '\x00',
        /* rx[0] waveform */
        '\x61', '\x62',
        /* rx[0] length */
        '\x02', '\x00',
        /* rx[0] waveform */
        '\x71', '\x72',
    };

    unsigned char p0_tx[] = { 0x30, 0x31 };
    unsigned char p0_rx0[] = { 0x40, 0x41, 0x42 };
    unsigned char p1_tx[] = { 0x51, 0x52 };
    unsigned char p1_rx0[] = { 0x61, 0x62 };
    unsigned char p1_rx1[] = { 0x71, 0x72 };
    struct eaarlio_pulse p0 = {
        1249809, 1, 0, 0, 0, 0, 0, { 0, 0, 0, 0 }, sizeof(p0_tx),
        { sizeof(p0_rx0), 0, 0, 0 }, (unsigned char *)&p0_tx,
        { (unsigned char *)&p0_rx0, NULL, NULL, NULL },
    };
    struct eaarlio_pulse p1 = {
        2302497, 2, 0, 0, 0, 0, 0, { 0, 0, 0, 0 }, sizeof(p1_tx),
        { sizeof(p1_rx0), sizeof(p1_rx1), 0, 0 }, (unsigned char *)&p1_tx,
        { (unsigned char *)&p1_rx0, (unsigned char *)&p1_rx1, NULL, NULL },
    };
    struct eaarlio_pulse pulses[] = { p0, p1 };
    struct eaarlio_raster raster = {
        67305985, 0, 0, 2, 0, (struct eaarlio_pulse *)&pulses,
    };

    *got = NULL;
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_tld_pack_raster(got, &got_len, &raster, NULL));

    ASSERT_EQ_FMT(sizeof exp, got_len, "%d");
    ASSERT(*got);
    ASSERT_MEM_EQ(&exp, *got, got_len);

    PASS();
}

SUITE(suite_raster)
{
    RUN_TEST(test_raster_sanity);
    RUN_TEST(test_raster_null_buffer);
    RUN_TEST(test_raster_null_buffer_len);
    RUN_TEST(test_raster_null_raster);

    {
        unsigned char *buf = NULL;
        RUN_TEST1(test_raster_values, &buf);
        if(buf)
            free(buf);
    }
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_raster);

    GREATEST_MAIN_END();
}
