#ifndef EAARLIO_MISC_SUPPORT_H
#define EAARLIO_MISC_SUPPORT_H

/**
 * @file
 * @brief Miscellaneous support code
 */

#include <limits.h>
#include <stdio.h>
#include <string.h>

/**
 * Bounded strlen
 *
 * Many platforms have a strlen, but it's not part of the standard so we
 * implement our own. This is to protect against strings that are not
 * null-terminated.
 */
size_t eaarlio_strnlen(char const *str, size_t max_len);

/**
 * If limits.h doesn't provide PATH_MAX, define our own
 */
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/**
 * Wrapper around fopen that forces binary mode on Windows
 */
FILE *eaarlio_fopenb(char const *filename, char const *mode);

#endif
