#include "utils.h"
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <utils.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "disp.h"

#define BT_SIZE 32

static FILE *open_assert_log(void) {
    const char *path = getenv("MED_ASSERT_LOG");
    if (path == NULL || path[0] == '\0') {
        path = "med_assert.log";
    }

    return fopen(path, "a");
}

void __assert_fail_bt(const char *expr, const char *file, int line) {
    FILE *assert_log = open_assert_log();

    fprintf(stderr, "Assertion failed: %s (%s:%d)\n", expr, file, line);
    if (assert_log != NULL) {
        fprintf(assert_log, "Assertion failed: %s (%s:%d)\n", expr, file, line);
        fflush(assert_log);
    }

    void *buf[BT_SIZE];
    int   nptrs = backtrace(buf, BT_SIZE);
    backtrace_symbols_fd(buf, nptrs, STDERR_FILENO);

    if (assert_log != NULL) {
        char **syms = backtrace_symbols(buf, nptrs);
        if (syms != NULL) {
            for (int i = 0; i < nptrs; i++) {
                fprintf(assert_log, "%s\n", syms[i]);
            }
            free(syms);
        } else {
            fprintf(assert_log, "backtrace_symbols failed (errno=%d)\n", errno);
        }
        fflush(assert_log);
        fclose(assert_log);
    }
    cleanup_display();
    exit(1);
}

#define assert_bt(expr) ((expr) ? (void)0 : __assert_fail_bt(#expr, __FILE__, __LINE__))

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
