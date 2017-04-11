#ifndef GREATEST_EXTEND_H
#define GREATEST_EXTEND_H

#include "greatest.h"

/* TEST includes the static qualifier. This excludes it so it can be used in
 * other contexts.
 */
#define TEST_TYPE enum greatest_test_res

/* Used at the beginning of a suite that uses RUN_MSG to create local variables
 */
#define RUN_MSG_INIT()                                                         \
    char __msg[1024];                                                          \
    char *__fmt;

/* Use prior to RUN_MSG to set the format specifier to use
 */
#define RUN_MSG_FORMAT(FMT) __fmt = FMT;

/* Run a test case, automatically formatting its args per RUN_MSG_FORMAT
 */
#define RUN_MSG(FUNC, ...)                                                     \
    sprintf(__msg, __fmt, __VA_ARGS__);                                        \
    RUN_TESTp(FUNC, __msg, __VA_ARGS__)

#endif
