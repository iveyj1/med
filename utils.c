#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <utils.h>
#include <assert.h>
#include <stdio.h>
#include "utils.h"

size_t str_copy_n(char *dst, size_t dstsz, const char *src) {
    size_t srclen = strlen(src);

    if (dstsz == 0)
        return srclen;

    size_t n = (srclen < dstsz - 1) ? srclen : dstsz - 1;

    memcpy(dst, src, n);
    dst[n] = '\0';

    return srclen;
}
size_t str_cat_n(char *dst, size_t dstsz, const char *src) {
    size_t dstlen = strlen(dst);
    size_t srclen = strlen(src);

    if (dstlen >= dstsz)
        return dstlen + srclen;

    size_t avail = dstsz - dstlen - 1;
    size_t n     = (srclen < avail) ? srclen : avail;

    memcpy(dst + dstlen, src, n);
    dst[dstlen + n] = '\0';

    return dstlen + srclen;
}

int str_len(const char *str, int buflen) {
    int i;
    for (i = 0; i < buflen; i++) {
        if (str[i] == '\0') {
            break;
        }
    }
    if (i == buflen) {
        return (-1);
    }
    return (i);
}

size_t get_file_size(FILE *file) {
    if (file == 0) {
        return (-1);
    }
    size_t size, pos;
    pos = ftell(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, pos, SEEK_SET);
    return (size);
}
