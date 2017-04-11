#include "eaarlio/tld_constants.h"
#include "greatest.h"

/*******************************************************************************
 * Constants
 *******************************************************************************
 */

/* These are intended as regression tests to catch accidental modification of
 * the constant values */

TEST test_constant_record_header_size()
{
    ASSERT_EQ_FMT(4, EAARLIO_TLD_RECORD_HEADER_SIZE, "%d");
    PASS();
}

TEST test_constant_raster_header_size()
{
    ASSERT_EQ_FMT(14, EAARLIO_TLD_RASTER_HEADER_SIZE, "%d");
    PASS();
}

TEST test_constant_raster_pulse_size()
{
    ASSERT_EQ_FMT(13, EAARLIO_TLD_PULSE_HEADER_SIZE, "%d");
    PASS();
}

TEST test_constant_wf_data_length_size()
{
    ASSERT_EQ_FMT(2, EAARLIO_TLD_WF_DATA_LENGTH_SIZE, "%d");
    PASS();
}

TEST test_constant_tx_length_size()
{
    ASSERT_EQ_FMT(1, EAARLIO_TLD_TX_LENGTH_SIZE, "%d");
    PASS();
}

TEST test_constant_rx_length_size()
{
    ASSERT_EQ_FMT(2, EAARLIO_TLD_RX_LENGTH_SIZE, "%d");
    PASS();
}

SUITE(suite_constants)
{
    RUN_TEST(test_constant_record_header_size);
    RUN_TEST(test_constant_raster_header_size);
    RUN_TEST(test_constant_raster_pulse_size);
    RUN_TEST(test_constant_wf_data_length_size);
    RUN_TEST(test_constant_tx_length_size);
    RUN_TEST(test_constant_rx_length_size);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_constants);

    GREATEST_MAIN_END();
}
