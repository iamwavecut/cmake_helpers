/*
 * Minimal iconv implementation for Windows.
 * Supports only conversions TO UTF-8 as required by zbar's qrdectxt.c.
 * Uses Windows MultiByteToWideChar / WideCharToMultiByte internally.
 */
#include "iconv.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    unsigned int from_cp;
    unsigned int to_cp;
} win_iconv_t;

static unsigned int name_to_cp(const char *name)
{
    if (!name)
        return (unsigned int)-1;
    if (!_stricmp(name, "UTF-8") || !_stricmp(name, "UTF8"))
        return CP_UTF8;
    if (!_stricmp(name, "SJIS") || !_stricmp(name, "SHIFT_JIS")
        || !_stricmp(name, "SHIFT-JIS"))
        return 932;
    if (!_stricmp(name, "BIG-5") || !_stricmp(name, "BIG5"))
        return 950;
    if (!_stricmp(name, "CP437"))
        return 437;
    /* ISO8859-1 .. ISO8859-16 */
    if (!_strnicmp(name, "ISO8859-", 8) || !_strnicmp(name, "ISO-8859-", 9)) {
        const char *p = strrchr(name, '-');
        if (p) {
            int n = atoi(p + 1);
            if (n >= 1 && n <= 16)
                return 28590 + n; /* 28591..28606 */
        }
    }
    return (unsigned int)-1;
}

iconv_t iconv_open(const char *tocode, const char *fromcode)
{
    unsigned int from_cp = name_to_cp(fromcode);
    unsigned int to_cp   = name_to_cp(tocode);
    win_iconv_t *cd;
    if (from_cp == (unsigned int)-1 || to_cp == (unsigned int)-1) {
        errno = EINVAL;
        return (iconv_t)-1;
    }
    cd = (win_iconv_t *)malloc(sizeof(win_iconv_t));
    if (!cd) {
        errno = ENOMEM;
        return (iconv_t)-1;
    }
    cd->from_cp = from_cp;
    cd->to_cp   = to_cp;
    return (iconv_t)cd;
}

size_t iconv(iconv_t cd_opaque, char **inbuf, size_t *inbytesleft,
             char **outbuf, size_t *outbytesleft)
{
    win_iconv_t *cd = (win_iconv_t *)cd_opaque;
    int wlen, olen;
    wchar_t *wbuf;

    if (!inbuf || !*inbuf)
        return 0; /* reset request, nothing to do */

    /* trivial case: same codepage, just copy */
    if (cd->from_cp == cd->to_cp) {
        size_t n = *inbytesleft < *outbytesleft ? *inbytesleft : *outbytesleft;
        memcpy(*outbuf, *inbuf, n);
        *inbuf += n; *inbytesleft -= n;
        *outbuf += n; *outbytesleft -= n;
        if (*inbytesleft > 0) { errno = E2BIG; return (size_t)-1; }
        return 0;
    }

    /* Step 1: source encoding -> UTF-16 */
    wlen = MultiByteToWideChar(cd->from_cp, 0, *inbuf, (int)*inbytesleft,
                               NULL, 0);
    if (wlen <= 0) { errno = EILSEQ; return (size_t)-1; }
    wbuf = (wchar_t *)malloc(wlen * sizeof(wchar_t));
    if (!wbuf) { errno = ENOMEM; return (size_t)-1; }
    MultiByteToWideChar(cd->from_cp, 0, *inbuf, (int)*inbytesleft, wbuf, wlen);

    /* Step 2: UTF-16 -> target encoding */
    olen = WideCharToMultiByte(cd->to_cp, 0, wbuf, wlen,
                               NULL, 0, NULL, NULL);
    if (olen <= 0) { free(wbuf); errno = EILSEQ; return (size_t)-1; }
    if ((size_t)olen > *outbytesleft) {
        free(wbuf);
        errno = E2BIG;
        return (size_t)-1;
    }
    WideCharToMultiByte(cd->to_cp, 0, wbuf, wlen,
                        *outbuf, olen, NULL, NULL);
    free(wbuf);

    *inbuf += *inbytesleft; *inbytesleft = 0;
    *outbuf += olen; *outbytesleft -= olen;
    return 0;
}

int iconv_close(iconv_t cd)
{
    free(cd);
    return 0;
}
