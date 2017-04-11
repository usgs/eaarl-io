#include "eaarlio/error.h"
#include "greatest.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* It's not practical to test that the name/message for each error case is
 * correct, since there are many and more might be added.
 *
 * The values for the fall-through/unknown error are known and should be
 * stable. They are defined below as UNKNOWN_NAME and UNKNOWN_MSG. The
 * translation functions are implemented such that an unknown or invalid value
 * should fall through to those. Conversely, those strings should not result
 * for any valid error case.
 *
 * The testing strategy then is thus:
 *      - Make sure we get the expected result for the unknown and invalid
 *        case.
 *      - Make sure that all of the valid cases do NOT return the unknown's
 *        string.
 * This ensures that all error codes are behaving within some known expected
 * parameters.
 */
#define UNKNOWN_NAME "(unknown)"
#define UNKNOWN_MSG "Unknown error"

/* -- Test cases for eaarlio_error_name */

/* Test EAARLIO_ERROR_UNKNOWN */
TEST test_name_unknown()
{
    ASSERT_STR_EQ(UNKNOWN_NAME, eaarlio_error_name(EAARLIO_ERROR_UNKNOWN));
    PASS();
}

/* Test invalid enum value */
TEST test_name_invalid()
{
    ASSERT_STR_EQ(UNKNOWN_NAME, eaarlio_error_name(EAARLIO_ERROR_UNKNOWN + 1));
    PASS();
}

/* Test EAARLIO_SUCCESS */
TEST test_name_success()
{
    ASSERT_STR_EQ("EAARLIO_SUCCESS", eaarlio_error_name(EAARLIO_SUCCESS));
    PASS();
}

/* Check to make sure a given enum value does not yield the value for unknown */
TEST test_name_check(eaarlio_error err)
{
    ASSERT(strcmp(UNKNOWN_NAME, eaarlio_error_name(err)) != 0);
    PASS();
}

SUITE(names)
{
    eaarlio_error err;

    RUN_TEST(test_name_unknown);
    RUN_TEST(test_name_invalid);
    RUN_TEST(test_name_success);

    for(err = EAARLIO_SUCCESS; err < EAARLIO_ERROR_UNKNOWN; err++) {
        RUN_TESTp(test_name_check, err);
    }
}

/* -- Test cases for eaarlio_error_message */

/* Test EAARLIO_ERROR_UNKNOWN */
TEST test_msg_unknown()
{
    ASSERT_STR_EQ(UNKNOWN_MSG, eaarlio_error_message(EAARLIO_ERROR_UNKNOWN));
    PASS();
}

/* Test invalid enum value */
TEST test_msg_invalid()
{
    ASSERT_STR_EQ(
        UNKNOWN_MSG, eaarlio_error_message(EAARLIO_ERROR_UNKNOWN + 1));
    PASS();
}

/* Check to make sure a given enum value does not yield the value for unknown */
TEST test_msg_check(eaarlio_error err)
{
    ASSERT(strcmp(UNKNOWN_MSG, eaarlio_error_message(err)) != 0);
    PASS();
}

SUITE(messages)
{
    eaarlio_error err;

    RUN_TEST(test_msg_unknown);
    RUN_TEST(test_msg_invalid);

    for(err = EAARLIO_SUCCESS; err < EAARLIO_ERROR_UNKNOWN; err++) {
        RUN_TESTp(test_msg_check, err);
    }
}

/* -- */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(names);
    RUN_SUITE(messages);

    GREATEST_MAIN_END();
}
