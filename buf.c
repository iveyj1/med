#include "buf.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "utils.h"
#include "disp.h"

int buf_chars(struct text_buf *buf) { return buf->cursor + buf->gap_buf_len - buf->back; }
int buf_gap_len(struct text_buf *buf) { return (buf->back - buf->cursor - 2); }

int buf_check(struct text_buf *buf) {
    assert_bt(buf->gap_buf != 0);
    assert_bt(buf->gap_buf_len > 0);
    assert_bt(buf->cursor >= -1);
    assert_bt(buf->gap_buf[buf->cursor + 1] == 0);
    assert_bt(buf->back > buf->cursor + 1);
    assert_bt(buf->back <= buf->gap_buf_len);
    assert_bt(buf->gap_buf[buf->gap_buf_len - 1] == 0);
    assert_bt(buf->target_gap_len > 0);
    return 0;
}

int buf_resize(struct text_buf *buf, size_t len) {
    // if (len >= buf->back - buf->cursor) {
    // }
    return 0;
}

int buf_dump(struct text_buf *buf) {
    for (int i = 0; i < buf->gap_buf_len; i++) {
        if (i % 32 == 0 && i != 0) {
            fprintf(logfile, "\n");
        }
        fprintf(logfile, "%02x ", (unsigned char)buf->gap_buf[i]);
    }
    fprintf(logfile, "\n");
    fflush(NULL);
    return 0;
}

int buf_status(struct text_buf *buf) {
    LOG(logfile,
        "Buffer status:\n\
 gap_buf %p\n\
 gap_buf_len %lu\n\
 target gap len %d\n\
 back %d\n\
 cursor %d\n\
 dirty %d\n\
 actual_gap %d\n\
 chars %d\n",
        (void *)buf->gap_buf, buf->gap_buf_len, buf->target_gap_len, buf->back, buf->cursor,
        buf->dirty, buf_gap_len(buf), buf_chars(buf));
    LOG(logfile, "Front:\n%s\nBack:\n%s\n", buf->gap_buf, buf->gap_buf + buf->back);
    return 0;
}

int buf_init(struct text_buf *buf, int text_size) {
    assert_bt(buf != 0);
    assert_bt(buf->gap_buf == 0);
    buf->target_gap_len = 256;
    buf->gap_buf_len    = text_size + buf->target_gap_len;
    buf->gap_buf        = calloc(buf->gap_buf_len, sizeof(char));
    memset(buf->gap_buf, 0xff, buf->gap_buf_len);
    buf->cursor                        = -1;
    buf->back                          = buf->gap_buf_len - 1;
    buf->dirty                         = false;
    buf->gap_buf[0]                    = '\0';
    buf->gap_buf[buf->gap_buf_len - 1] = '\0';
    buf_status(buf);
    buf_dump(buf);
    buf_check(buf);
    return 0;
}

int buf_get_char(struct text_buf *buf, int index) {
    char c;
    assert_bt(index < buf_chars(buf));
    if (index <= buf->cursor) {
        c = buf->gap_buf[index];
        LOG(logfile, "buf_get_char index in front %d %c %x\n", index, c, c);
    } else {
        c = buf->gap_buf[index - buf->cursor + buf->back - 1];
        LOG(logfile, "buf_get_char index in back %d %c %x\n", index, c, c);
    }
    return c;
}

int buf_append(struct text_buf *buf, const char *str, size_t maxstrlen) {
    buf_check(buf);
    assert_bt(strlen(str) <= buf_gap_len(buf));
    int len = str_len(str, maxstrlen);
    buf_resize(buf, len);
    memcpy(buf->gap_buf + buf->cursor + 1, str, len);
    buf->cursor += len;
    buf->gap_buf[buf->cursor + 1] = '\0';
    buf->dirty                    = true;
    buf_check(buf);
    return 0;
}

int buf_seek(struct text_buf *buf, int pos) {
    buf_check(buf);
    int chars = buf_chars(buf);
    assert_bt(pos < chars && pos >= -1);
    int   move = pos - buf->cursor;
    char *dest, *src;
    if (move < 0) {
        src  = buf->gap_buf + pos + 1;
        dest = buf->gap_buf + buf->back + move;
        memcpy(dest, src, abs(move));
        memset(src, 0xff, abs(move) + 1);
    } else if (move > 0) {
        src  = buf->gap_buf + buf->back;
        dest = buf->gap_buf + buf->cursor + 1;
        memcpy(dest, src, move);
        memset(src, 0xff, abs(move));
    }
    buf->cursor = pos;
    buf->back += move;
    buf->gap_buf[buf->cursor + 1]      = '\0';
    buf->gap_buf[buf->gap_buf_len - 1] = '\0';
    buf_check(buf);
    return 0;
}

// // fills a buffer with one line from the text buffer starting from startindex
// int buf_getline(struct text_buf *buf, int startindex, char *line, int linebuflen) {
//     if (buf == 0) {
//         show_status("No buffer in buffer_getline");
//         return -1;
//     }
//     int i;
//     for (i = 0; i < linebuflen - 1; i++) {
//         if (startindex + i < buf->cursor) {
//             line[i] = buf->gap_buf[startindex + i];
//         } else {
//             line[i] = buf->gap_buf[buf->cursor - startindex + buf->back + i];
//         }
//         if (line[i] == '\n') {
//             break;
//         }
//     }
//     line[i + 1] = '\0';
//     return i;
// }

int buf_open(struct text_buf *buf, const char *filename, FILE *file) {
    size_t size = 65536;
    assert_bt(buf != 0);
    assert_bt(strlen(filename) > 0);
    if (strlen(filename) > 0) {
        file = fopen(filename, "rw+");
        if (file == 0) {
            show_status("buf_open: can't open %s", filename);
            return -1;
        }
        size = get_file_size(file);
    }
    assert_bt(buf_init(buf, size) >= 0);
    buf_check(buf);
    return size;
}
