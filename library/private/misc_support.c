#include "eaarlio/misc_support.h"

size_t eaarlio_strnlen(char const *str, size_t max_len)
{
    const char *end = (const char *)memchr(str, '\0', max_len);
    if(!end)
        return max_len;
    return end - str;
}

#define _EAARLIO_FOPENB_BUFLEN 16

FILE *eaarlio_fopenb(char const *filename, char const *mode)
{
#ifdef _WIN32
    char const *buffer[_EAARLIO_FOPENB_BUFLEN];
    if(eaarlio_strnlen(mode, _EAARLIO_FOPENB_BUFLEN) < _EAARLIO_FOPENB_BUFLEN
        && !strchr(mode, 'b')) {
        strcpy((char *)buffer, mode);
        strcat((char *)buffer, "b");
        mode = (char const *)buffer;
    }
#endif
    return fopen(filename, mode);
}
