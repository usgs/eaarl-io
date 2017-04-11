#ifndef UTIL_ERROR_H
#define UTIL_ERROR_H

#include "eaarlio/error.h"
#include "greatest.h"

extern greatest_type_info eaarlio_error_type_info;

#define ASSERT_EAARLIO_ERR(exp, got)                                           \
    do {                                                                       \
        eaarlio_error __exp__ = (exp);                                         \
        eaarlio_error __got__ = (got);                                         \
        ASSERT_EQUAL_T(&__exp__, &__got__, &eaarlio_error_type_info, NULL);    \
    } while(0)

#define ASSERT_EAARLIO_ERRm(msg, exp, got)                                     \
    do {                                                                       \
        eaarlio_error __exp__ = (exp);                                         \
        eaarlio_error __got__ = (got);                                         \
        ASSERT_EQUAL_Tm(                                                       \
            (msg), &__exp__, &__got__, &eaarlio_error_type_info, NULL);        \
    } while(0)

#define ASSERT_EAARLIO_SUCCESS(got) ASSERT_EAARLIO_ERR(EAARLIO_SUCCESS, (got))

#define ASSERT_EAARLIO_SUCCESSm(msg, got)                                      \
    ASSERT_EAARLIO_ERRm((msg), EAARLIO_SUCCESS, (got))

#endif
