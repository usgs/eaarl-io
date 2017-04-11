/* malloc, srand, rand */
#include <stdlib.h>
/* time, difftime */
#include <time.h>
/* strcpy, memset */
#include <string.h>

#include "util_tempfile.h"

#define TEMPFILE_TEMPLATE (TEMPDIR "/tmp-XXXXXX")
#define TEMPFILE_MAX (sizeof(TEMPFILE_TEMPLATE))

char *util_tempfile()
{
    static unsigned long seed = 0;
    char *path = malloc(TEMPFILE_MAX);
    if(!path)
        return NULL;
    strcpy(path, TEMPFILE_TEMPLATE);

    /* Only initialize the seed once */
    if(!seed) {
        /* This is intended to be portable. While time_t is almost always the
         * integer seconds of the epoch, that is not actually guaranteed. Thus
         * a small hoop is jumped through to cast it to such.
         */
        time_t zero, now = time(NULL);
        memset(&zero, 0, sizeof zero);
        seed = (unsigned long)difftime(now, zero);
        /* In the incredibly unlikely event that the above results in seed = 0,
         * force it to 1 to prevent repeatedly calling srand. (This is probably
         * overly paranoid, but the cost is negligible.)
         */
        seed = seed > 0 ? seed : 1;
        srand(seed);
    }

    /* Randomize trailing X characters. This isn't the most random or uniform
     * outcome possible, but it's sufficient for portability.
     */
    size_t i = TEMPFILE_MAX - 2;
    while(path[i] == 'X')
        path[i--] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand() % 26];

    return path;
}
