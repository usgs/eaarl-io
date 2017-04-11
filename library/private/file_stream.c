#include <assert.h>
#include <stdio.h>

#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/misc_support.h"
#include "eaarlio/stream.h"

static eaarlio_error eaarlio_file_stream_close(struct eaarlio_stream *self)
{
    if(!self)
        return EAARLIO_NULL;
    if(!self->data)
        return EAARLIO_STREAM_INVALID;

    FILE *f = (FILE *)self->data;

    if(fclose(f))
        return EAARLIO_STREAM_CLOSE_ERROR;

    *self = eaarlio_stream_empty();

    return EAARLIO_SUCCESS;
}

static eaarlio_error eaarlio_file_stream_read(struct eaarlio_stream *self,
    uint64_t len,
    unsigned char *buf)
{
    if(!self)
        return EAARLIO_NULL;
    if(!buf)
        return EAARLIO_NULL;
    if(!self->data)
        return EAARLIO_STREAM_INVALID;
    if(len == 0)
        return EAARLIO_SUCCESS;

    FILE *f = (FILE *)self->data;
    size_t bytes = fread(buf, 1, len, f);

    if(ferror(f)) {
        clearerr(f);
        return EAARLIO_STREAM_READ_ERROR;
    }

    if(bytes < len)
        return EAARLIO_STREAM_READ_SHORT;

    assert(bytes == len);

    return EAARLIO_SUCCESS;
}

static eaarlio_error eaarlio_file_stream_write(struct eaarlio_stream *self,
    uint64_t len,
    unsigned char const *buf)
{
    if(!self)
        return EAARLIO_NULL;
    if(!buf)
        return EAARLIO_NULL;
    if(!self->data)
        return EAARLIO_STREAM_INVALID;
    if(len == 0)
        return EAARLIO_SUCCESS;

    FILE *f = (FILE *)self->data;
    size_t bytes = fwrite(buf, 1, len, f);

    if(ferror(f)) {
        clearerr(f);
        return EAARLIO_STREAM_WRITE_ERROR;
    }

    if(bytes < len)
        return EAARLIO_STREAM_WRITE_SHORT;

    return EAARLIO_SUCCESS;
}

static eaarlio_error eaarlio_file_stream_seek(struct eaarlio_stream *self,
    int64_t offset,
    int whence)
{
    if(!self)
        return EAARLIO_NULL;
    if(!self->data)
        return EAARLIO_STREAM_INVALID;

    int fail = 1;
    FILE *f = (FILE *)self->data;

    if((long)offset != offset)
        return EAARLIO_STREAM_SEEK_ERROR;

    switch(whence) {
        case SEEK_SET:
        case SEEK_CUR:
        case SEEK_END:
            fail = fseek(f, (long)offset, whence);
            break;
        default:
            return EAARLIO_STREAM_SEEK_INVALID;
    }

    if(fail)
        return EAARLIO_STREAM_SEEK_ERROR;

    return EAARLIO_SUCCESS;
}

static eaarlio_error eaarlio_file_stream_tell(struct eaarlio_stream *self,
    int64_t *position)
{
    if(!self)
        return EAARLIO_NULL;
    if(!self->data)
        return EAARLIO_STREAM_INVALID;
    if(!position)
        return EAARLIO_NULL;

    FILE *f = (FILE *)self->data;

    *position = ftell(f);

    if(ferror(f)) {
        clearerr(f);
        return EAARLIO_STREAM_TELL_ERROR;
    }

    if(*position < 0)
        return EAARLIO_STREAM_TELL_ERROR;

    return EAARLIO_SUCCESS;
}

#define _EAARLIO_FILE_STREAM_BUFLEN 16
eaarlio_error eaarlio_file_stream(struct eaarlio_stream *stream,
    char const *fn,
    char const *mode)
{
    FILE *f;

    if(!stream)
        return EAARLIO_NULL;

    *stream = eaarlio_stream_empty();

    if(!fn)
        return EAARLIO_NULL;
    if(!mode)
        return EAARLIO_NULL;

    f = eaarlio_fopenb(fn, mode);

    if(!f)
        return EAARLIO_STREAM_OPEN_ERROR;

    stream->close = &eaarlio_file_stream_close;
    stream->read = &eaarlio_file_stream_read;
    stream->write = &eaarlio_file_stream_write;
    stream->seek = &eaarlio_file_stream_seek;
    stream->tell = &eaarlio_file_stream_tell;
    stream->data = (void *)f;

    return EAARLIO_SUCCESS;
}
