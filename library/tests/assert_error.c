#include "assert_error.h"
#include "eaarlio/error.h"

static int _error_equal_cb(const void *exp, const void *got, void *udata)
{
    eaarlio_error *e = (eaarlio_error *)exp;
    eaarlio_error *g = (eaarlio_error *)got;
    (void)udata;
    return (*e == *g);
}

static int _error_print_cb(const void *t, void *udata)
{
    eaarlio_error *err = (eaarlio_error *)t;
    (void)udata;
    return printf("%s", eaarlio_error_name(*err));
}

greatest_type_info eaarlio_error_type_info = { _error_equal_cb,
    _error_print_cb };
