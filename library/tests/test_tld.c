#include "eaarlio/tld.h"
#include "greatest.h"

/*******************************************************************************
 * Constants
 *******************************************************************************
 */

/* These are intended as regression tests to catch accidental modification of
 * the constant values */

TEST test_constant_type_raster()
{
    ASSERT_EQ_FMT(5, EAARLIO_TLD_TYPE_RASTER, "%d");
    PASS();
}

SUITE(suite_constants)
{
    RUN_TEST(test_constant_type_raster);
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
