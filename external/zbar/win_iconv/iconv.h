/*
 * Minimal iconv shim for Windows using MultiByteToWideChar/WideCharToMultiByte.
 * Covers only the subset used by zbar: one-directional conversion to UTF-8.
 */
#ifndef WIN_ICONV_H
#define WIN_ICONV_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *iconv_t;

iconv_t iconv_open(const char *tocode, const char *fromcode);
size_t  iconv(iconv_t cd, char **inbuf, size_t *inbytesleft,
              char **outbuf, size_t *outbytesleft);
int     iconv_close(iconv_t cd);

#ifdef __cplusplus
}
#endif

#endif /* WIN_ICONV_H */
