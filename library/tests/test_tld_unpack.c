#include "eaarlio/memory_support.h"
#include "eaarlio/tld.h"
#include "eaarlio/tld_encode.h"
#include "eaarlio/tld_unpack.h"
#include "greatest.h"
#include "assert_error.h"
#include <stdint.h>
#include <stdio.h>

#ifdef _MSC_VER
/* Visual Studio complains about the unsigned char initialization */
#pragma warning(disable : 4245)
#endif

/*******************************************************************************
 * Helper for out-of-memory errors
 *******************************************************************************
 */

struct mock_calloc_oom_data {
    uint_fast8_t allowed;
    struct eaarlio_pulse pulse;
};

static void *mock_calloc_oom(struct eaarlio_memory *self,
    size_t nmemb,
    size_t size)
{
    struct mock_calloc_oom_data *data =
        (struct mock_calloc_oom_data *)self->opaque;
    if(data->allowed) {
        data->allowed -= 1;
        return calloc(nmemb, size);
    } else {
        return NULL;
    }
}

/*******************************************************************************
 * eaarlio_tld_unpack_waveforms
 *******************************************************************************
 */

TEST test_waveforms_sanity()
{
    eaarlio_tld_unpack_waveforms(NULL, 0, NULL, NULL);
    PASS();
}

TEST test_waveforms_null_buffer()
{
    struct eaarlio_pulse pulse;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_unpack_waveforms(NULL, 1, &pulse, NULL));
    PASS();
}

TEST test_waveforms_null_pulse()
{
    unsigned char buffer[1];
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_unpack_waveforms((unsigned char *)&buffer, 1, NULL, NULL));
    PASS();
}

/* Can we decode values?
 * Do we ignore extra waveform data if beyond waveform count?
 * Do we properly handle zero-length waveforms?
 */
TEST test_waveforms_values(struct eaarlio_pulse *pulse)
{
    unsigned char const buf[] = { /* tx length */
        '\x02',
        /* tx waveform */
        '\x10', '\x11',
        /* rx[0] length */
        '\x03', '\x00',
        /* rx[0] waveform */
        '\x20', '\x21', '\x22',
        /* rx[1] length */
        '\x00', '\x00',
        /* rx[1] waveform */
        /* no data */
        /* rx[2] length */
        '\x04', '\x00',
        /* rx[2] waveform */
        '\x30', '\x31', '\x32', '\x33',
        /* rx[3] length */
        '\x02', '\x00',
        /* rx[3] waveform */
        '\x40', '\x41'
    };

    memset(pulse, 0, sizeof(struct eaarlio_pulse));
    pulse->tx = NULL;
    pulse->rx[0] = NULL;
    pulse->rx[1] = NULL;
    pulse->rx[2] = NULL;
    pulse->rx[3] = NULL;

    pulse->rx_count = 3;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_unpack_waveforms(
        (unsigned char const *)buf, sizeof buf, pulse, NULL));

    ASSERT_EQ_FMT(2, pulse->tx_len, "%d");
    ASSERT_EQ_FMT(3, pulse->rx_len[0], "%d");
    ASSERT_EQ_FMT(0, pulse->rx_len[1], "%d");
    ASSERT_EQ_FMT(4, pulse->rx_len[2], "%d");
    ASSERT_EQ_FMT(0, pulse->rx_len[3], "%d");

    ASSERT(pulse->tx);
    ASSERT(pulse->rx[0]);
    ASSERT_FALSE(pulse->rx[1]);
    ASSERT(pulse->rx[2]);
    ASSERT_FALSE(pulse->rx[3]);

    ASSERT_EQ_FMT(0x10U, pulse->tx[0], "%02x");
    ASSERT_EQ_FMT(0x11U, pulse->tx[1], "%02x");
    ASSERT_EQ_FMT(0x20U, pulse->rx[0][0], "%02x");
    ASSERT_EQ_FMT(0x21U, pulse->rx[0][1], "%02x");
    ASSERT_EQ_FMT(0x22U, pulse->rx[0][2], "%02x");
    ASSERT_EQ_FMT(0x30U, pulse->rx[2][0], "%02x");
    ASSERT_EQ_FMT(0x31U, pulse->rx[2][1], "%02x");
    ASSERT_EQ_FMT(0x32U, pulse->rx[2][2], "%02x");
    ASSERT_EQ_FMT(0x33U, pulse->rx[2][3], "%02x");

    PASS();
}

/* Do we properly handle a truncated rx? */
TEST test_waveforms_trunc(struct eaarlio_pulse *pulse)
{
    unsigned char const buf[] = { /* tx length */
        '\x02',
        /* tx waveform */
        '\x10', '\x11',
        /* rx[0] length */
        '\x05', '\x00',
        /* rx[0] waveform */
        '\x20', '\x21', '\x22'
    };

    memset(pulse, 0, sizeof(struct eaarlio_pulse));
    pulse->tx = NULL;
    pulse->rx[0] = NULL;
    pulse->rx[1] = NULL;
    pulse->rx[2] = NULL;
    pulse->rx[3] = NULL;

    pulse->rx_count = 3;

    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_unpack_waveforms(
            (unsigned char const *)buf, sizeof buf, pulse, NULL));

    ASSERT_EQ_FMT(3, pulse->rx_len[0], "%d");
    ASSERT(pulse->rx[0]);

    ASSERT_EQ_FMT(0x20U, pulse->rx[0][0], "%02x");
    ASSERT_EQ_FMT(0x21U, pulse->rx[0][1], "%02x");
    ASSERT_EQ_FMT(0x22U, pulse->rx[0][2], "%02x");

    PASS();
}

/* Do we handle out-of-memory correctly for tx? */
TEST test_waveforms_oom_tx(struct eaarlio_memory *mem)
{
    unsigned char const buf[] = { /* tx length */
        '\x02',
        /* tx waveform */
        '\x10', '\x11',
        /* rx[0] length */
        '\x05', '\x00',
        /* rx[0] waveform */
        '\x20', '\x21', '\x22', '\x23', '\x24'
    };
    struct mock_calloc_oom_data *mock =
        (struct mock_calloc_oom_data *)mem->opaque;
    struct eaarlio_pulse *pulse = &mock->pulse;

    memset(pulse, 0, sizeof(struct eaarlio_pulse));
    pulse->tx = NULL;
    pulse->rx[0] = NULL;
    pulse->rx[1] = NULL;
    pulse->rx[2] = NULL;
    pulse->rx[3] = NULL;

    pulse->rx_count = 1;

    ASSERT_EAARLIO_ERR(EAARLIO_MEMORY_ALLOC_FAIL,
        eaarlio_tld_unpack_waveforms(
            (unsigned char const *)buf, sizeof buf, pulse, mem));

    ASSERT_FALSE(pulse->tx);

    PASS();
}

/* Do we handle out-of-memory correctly for rx? */
TEST test_waveforms_oom_rx(struct eaarlio_memory *mem)
{
    unsigned char const buf[] = { /* tx length */
        '\x02',
        /* tx waveform */
        '\x10', '\x11',
        /* rx[0] length */
        '\x05', '\x00',
        /* rx[0] waveform */
        '\x20', '\x21', '\x22', '\x23', '\x24'
    };
    struct mock_calloc_oom_data *mock =
        (struct mock_calloc_oom_data *)mem->opaque;
    struct eaarlio_pulse *pulse = &mock->pulse;

    memset(pulse, 0, sizeof(struct eaarlio_pulse));
    pulse->tx = NULL;
    pulse->rx[0] = NULL;
    pulse->rx[1] = NULL;
    pulse->rx[2] = NULL;
    pulse->rx[3] = NULL;

    pulse->rx_count = 1;

    ASSERT_EAARLIO_ERR(EAARLIO_MEMORY_ALLOC_FAIL,
        eaarlio_tld_unpack_waveforms(
            (unsigned char const *)buf, sizeof buf, pulse, mem));

    ASSERT(pulse->tx);
    ASSERT_FALSE(pulse->rx[0]);

    PASS();
}

SUITE(suite_waveforms)
{
    RUN_TEST(test_waveforms_sanity);
    RUN_TEST(test_waveforms_null_buffer);
    RUN_TEST(test_waveforms_null_pulse);

    {
        struct eaarlio_pulse pulse;

        RUN_TESTp(test_waveforms_values, &pulse);
        eaarlio_pulse_free(&pulse, NULL);

        RUN_TESTp(test_waveforms_trunc, &pulse);
        eaarlio_pulse_free(&pulse, NULL);
    }

    {
        struct eaarlio_memory mem = eaarlio_memory_default;
        struct mock_calloc_oom_data data;
        mem.opaque = (void *)&data;
        mem.calloc = mock_calloc_oom;

        data.allowed = 0;
        RUN_TESTp(test_waveforms_oom_tx, &mem);
        eaarlio_pulse_free(&data.pulse, &mem);

        data.allowed = 1;
        RUN_TESTp(test_waveforms_oom_rx, &mem);
        eaarlio_pulse_free(&data.pulse, &mem);
    }
}

/*******************************************************************************
 * eaarlio_tld_unpack_pulses
 *******************************************************************************
 */

TEST test_pulses_sanity()
{
    eaarlio_tld_unpack_pulses(NULL, 0, NULL, NULL, 0);
    PASS();
}

TEST test_pulses_null_buffer()
{
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_unpack_pulses(NULL, 1, &raster, NULL, 0));
    PASS();
}

TEST test_pulses_null_raster()
{
    unsigned char buffer[1];
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_unpack_pulses((unsigned char *)&buffer, 1, NULL, NULL, 0));
    PASS();
}

/* Can we decode values?
 * Do we properly handle multiple pulses?
 * Do we properly handle if the buffer is longer than needed?
 */
TEST test_pulses_values(struct eaarlio_raster *raster)
{
    unsigned char const buf[] = { /* pulse[0] */
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
        '\x07', '\x00',
        /* tx length */
        '\x02',
        /* tx waveform */
        '\x51', '\x52',
        /* rx[0] length */
        '\x02', '\x00',
        /* rx[0] waveform */
        '\x61', '\x62',

        /* extra garbage */
        '\xAA', '\xBB', '\xCC', '\xDD'
    };

    memset(raster, 0, sizeof(struct eaarlio_raster));
    raster->pulse = NULL;
    raster->pulse_count = 2;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_unpack_pulses(
        (unsigned char const *)buf, sizeof buf, raster, NULL, 1));

    ASSERT(raster->pulse);

    ASSERT_EQ_FMT(1249809, raster->pulse[0].time_offset, "%d");
    ASSERT_EQ_FMT(1, raster->pulse[0].rx_count, "%d");
    ASSERT_EQ_FMT(2, raster->pulse[0].tx_len, "%d");
    ASSERT_EQ_FMT(3, raster->pulse[0].rx_len[0], "%d");
    ASSERT_EQ_FMT(0x30, raster->pulse[0].tx[0], "%02x");
    ASSERT_EQ_FMT(0x31, raster->pulse[0].tx[1], "%02x");
    ASSERT_EQ_FMT(0x40, raster->pulse[0].rx[0][0], "%02x");
    ASSERT_EQ_FMT(0x41, raster->pulse[0].rx[0][1], "%02x");
    ASSERT_EQ_FMT(0x42, raster->pulse[0].rx[0][2], "%02x");

    ASSERT_EQ_FMT(2302497, raster->pulse[1].time_offset, "%d");
    ASSERT_EQ_FMT(1, raster->pulse[1].rx_count, "%d");
    ASSERT_EQ_FMT(2, raster->pulse[1].tx_len, "%d");
    ASSERT_EQ_FMT(2, raster->pulse[1].rx_len[0], "%d");
    ASSERT_EQ_FMT(0x51, raster->pulse[1].tx[0], "%02x");
    ASSERT_EQ_FMT(0x52, raster->pulse[1].tx[1], "%02x");
    ASSERT_EQ_FMT(0x61, raster->pulse[1].rx[0][0], "%02x");
    ASSERT_EQ_FMT(0x62, raster->pulse[1].rx[0][1], "%02x");

    PASS();
}

SUITE(suite_pulses)
{
    struct eaarlio_raster raster = eaarlio_raster_empty();

    RUN_TEST(test_pulses_sanity);
    RUN_TEST(test_pulses_null_buffer);
    RUN_TEST(test_pulses_null_raster);

    RUN_TESTp(test_pulses_values, &raster);
    eaarlio_raster_free(&raster, NULL);
}

/*******************************************************************************
 * eaarlio_tld_unpack_raster
 *******************************************************************************
 */

TEST test_raster_sanity()
{
    eaarlio_tld_unpack_raster(NULL, 0, NULL, NULL, 0, 0);
    PASS();
}

TEST test_raster_null_buffer()
{
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_unpack_raster(NULL, 1, &raster, NULL, 0, 0));
    PASS();
}

TEST test_raster_null_raster()
{
    unsigned char buffer[1];
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_unpack_raster(
                          (unsigned char *)&buffer, 1, NULL, NULL, 0, 0));
    PASS();
}

/* Can we decode values?
 * Do we properly handle multiple pulses?
 * Do we properly handle if the buffer is longer than needed?
 */
TEST test_raster_values(struct eaarlio_raster *raster)
{
    unsigned char const buf[] = { /* raster_header */
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
        '\x07', '\x00',
        /* tx length */
        '\x02',
        /* tx waveform */
        '\x51', '\x52',
        /* rx[0] length */
        '\x02', '\x00',
        /* rx[0] waveform */
        '\x61', '\x62',

        /* extra garbage */
        '\xAA', '\xBB', '\xCC', '\xDD'
    };

    memset(raster, 0, sizeof(struct eaarlio_raster));
    raster->pulse = NULL;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_unpack_raster(
        (unsigned char const *)buf, sizeof buf, raster, NULL, 1, 1));

    ASSERT_EQ_FMT(67305985, raster->time_seconds, "%d");
    ASSERT_EQ_FMT(2, raster->pulse_count, "%d");
    ASSERT(raster->pulse);

    ASSERT_EQ_FMT(1249809, raster->pulse[0].time_offset, "%d");
    ASSERT_EQ_FMT(1, raster->pulse[0].rx_count, "%d");
    ASSERT_EQ_FMT(2, raster->pulse[0].tx_len, "%d");
    ASSERT_EQ_FMT(3, raster->pulse[0].rx_len[0], "%d");
    ASSERT_EQ_FMT(0x30, raster->pulse[0].tx[0], "%02x");
    ASSERT_EQ_FMT(0x31, raster->pulse[0].tx[1], "%02x");
    ASSERT_EQ_FMT(0x40, raster->pulse[0].rx[0][0], "%02x");
    ASSERT_EQ_FMT(0x41, raster->pulse[0].rx[0][1], "%02x");
    ASSERT_EQ_FMT(0x42, raster->pulse[0].rx[0][2], "%02x");

    ASSERT_EQ_FMT(2302497, raster->pulse[1].time_offset, "%d");
    ASSERT_EQ_FMT(1, raster->pulse[1].rx_count, "%d");
    ASSERT_EQ_FMT(2, raster->pulse[1].tx_len, "%d");
    ASSERT_EQ_FMT(2, raster->pulse[1].rx_len[0], "%d");
    ASSERT_EQ_FMT(0x51, raster->pulse[1].tx[0], "%02x");
    ASSERT_EQ_FMT(0x52, raster->pulse[1].tx[1], "%02x");
    ASSERT_EQ_FMT(0x61, raster->pulse[1].rx[0][0], "%02x");
    ASSERT_EQ_FMT(0x62, raster->pulse[1].rx[0][1], "%02x");

    PASS();
}

SUITE(suite_raster)
{
    struct eaarlio_raster raster = eaarlio_raster_empty();

    RUN_TEST(test_raster_sanity);
    RUN_TEST(test_raster_null_buffer);
    RUN_TEST(test_raster_null_raster);

    RUN_TESTp(test_raster_values, &raster);
    eaarlio_raster_free(&raster, NULL);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_waveforms);
    RUN_SUITE(suite_pulses);
    RUN_SUITE(suite_raster);

    GREATEST_MAIN_END();
}
