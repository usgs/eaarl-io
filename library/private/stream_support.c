#include "eaarlio/stream_support.h"
#include <stdlib.h>

int eaarlio_stream_valid(struct eaarlio_stream const *stream)
{
    return stream && stream->close && stream->read && stream->write
        && stream->seek && stream->tell;
}
