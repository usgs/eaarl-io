#include "eaarlio/edb_internals.h"
#include "greatest.h"

/*******************************************************************************
 * Constants
 *******************************************************************************
 */

/* These are intended as regression tests to catch accidental modification of
 * the constant values. */

TEST test_constant_header_size()
{
    ASSERT_EQ_FMT(12, EAARLIO_EDB_HEADER_SIZE, "%d");
    PASS();
}

TEST test_constant_record_size()
{
    ASSERT_EQ_FMT(20, EAARLIO_EDB_RECORD_SIZE, "%d");
    PASS();
}

TEST test_constant_filename_length_size()
{
    ASSERT_EQ_FMT(2, EAARLIO_EDB_FILENAME_LENGTH_SIZE, "%d");
    PASS();
}

SUITE(suite_constants)
{
    RUN_TEST(test_constant_header_size);
    RUN_TEST(test_constant_record_size);
    RUN_TEST(test_constant_filename_length_size);
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
