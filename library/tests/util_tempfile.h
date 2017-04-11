#ifndef UTIL_TEMPFILE_H
#define UTIL_TEMPFILE_H

#ifndef TEMPDIR
#ifdef DATADIR
#define TEMPDIR DATADIR
#else
#define TEMPDIR "data"
#endif
#endif

/**
 * Generate a random path
 *
 * This is intended to be a portable replacement for tmpnam. The tempnam
 * function generates warnings because of race conditions and security
 * vulnerabilities. This function suffers from those same issues, but doesn't
 * generate the warnings/errors. But for the purposes of our test cases, it
 * should be safe enough.
 *
 * As a side effect, it invokes srand on its first call.
 */
char *util_tempfile();

#endif
