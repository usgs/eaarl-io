#ifndef ASSERT_RANGE_H
#define ASSERT_RANGE_H

/* pow */
#include <math.h>
/* DBL_DIG */
#include <float.h>

#define ASSERT_IN_RANGE_POWER(EXP, GOT, TOL)                                   \
    ASSERT_IN_RANGE((EXP), (GOT), pow(10., (TOL)-DBL_DIG));

#define ASSERT_IN_RANGE_POWERm(MSG, EXP, GOT, TOL)                             \
    ASSERT_IN_RANGEm((MSG), (EXP), (GOT), pow(10., (TOL)-DBL_DIG));

#endif
