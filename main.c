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

FILE *logfile;
FILE *main_file            = 0;
char  main_file_name[1024] = "";
enum mode { NORMAL, INSERT, COMMAND };

struct text_buf {
    char  *gap_buf;
    size_t gap_buf_len;
    int    cursor;
    int    back;
    int    dirty;
    int    target_gap_len;
};

struct text_buf main_buf = {0, 0, 0, 0, 0};

int buf_chars(struct text_buf *buf) { return buf->cursor + buf->gap_buf_len - buf->back; }

int buf_resize(struct text_buf *buf, size_t len) {
    if (len >= buf->back - buf->cursor) {
    }
    return 0;
}

int buf_init(struct text_buf *buf, int text_size) {
    assert(buf != 0);
    assert(buf->gap_buf == 0);
    buf->target_gap_len                = 1000;
    buf->gap_buf_len                   = text_size + buf->target_gap_len;
    buf->gap_buf                       = calloc(buf->gap_buf_len, sizeof(char));
    buf->cursor                        = 0;
    buf->back                          = buf->gap_buf_len - 1;
    buf->dirty                         = false;
    buf->gap_buf[0]                    = '\0';
    buf->gap_buf[buf->gap_buf_len - 1] = '\0';
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
        buf->dirty, buf->back - buf->cursor, buf_chars(buf));
    char *start_of_back_buffer = buf->gap_buf + buf->back;
    LOG(logfile, "Front:\n%s\nBack:\n%s\n", buf->gap_buf, start_of_back_buffer);
    return 0;
}

int buf_dump(struct text_buf *buf) {
    for (int i = 0; i < buf->gap_buf_len; i++) {
        if (i % 15 == 0 && i != 0) {
            fprintf(logfile, "\n");
        }
        fprintf(logfile, "%02x ", buf->gap_buf[i]);
    }
    fprintf(logfile, "\n");
    return 0;
}

int buf_insert(struct text_buf *buf, const char *str, size_t maxstrlen) {
    assert(buf != 0);
    int len = str_len(str, maxstrlen);
    buf_resize(buf, len);
    memcpy(buf->gap_buf + buf->cursor, str, len);
    buf->cursor += len;
    buf->gap_buf[buf->cursor] = '\0';
    return 0;
}

int buf_seek(struct text_buf *buf, int pos) {
    assert(buf != 0);
    int chars = buf_chars(buf);
    assert(pos < chars && pos >= 0);
    int move = pos - buf->cursor;
    assert(move < 0);
    if (move < 0) {
        char *src  = buf->gap_buf + pos;
        char *dest = buf->gap_buf + buf->back + move;
        int   num  = -1 * move;
        memcpy(dest, src, num);
        buf->cursor = pos;
        buf->back += move;
        buf->gap_buf[buf->cursor]          = '\0';
        buf->gap_buf[buf->gap_buf_len - 1] = '\0';
    }
}
// fills a buffer with one line from the text buffer starting from startindex
int buf_getline(struct text_buf *buf, int startindex, char *line, int linebuflen) {
    if (buf == 0) {
        show_status("No buffer in buffer_getline");
        return -1;
    }
    int i;
    for (i = 0; i < linebuflen - 1; i++) {
        if (startindex + i < buf->cursor) {
            line[i] = buf->gap_buf[startindex + i];
        } else {
            line[i] = buf->gap_buf[buf->cursor - startindex + buf->back + i];
        }
        if (line[i] == '\n') {
            break;
        }
    }
    return i;
}

int buf_open(struct text_buf *buf, const char *filename, FILE *file) {
    size_t size = 65536;
    assert(buf != 0);
    if (strlen(filename) > 0) {
        file = fopen(filename, "rw+");
        if (file == 0) {
            show_status("buf_open: can't open %s", filename);
            return -1;
        }
        size = get_file_size(file);
    }
    assert(buf_init(buf, size) >= 0);
    assert(size == buf->cursor + buf->gap_buf_len - buf->back);
    return size;
}

int edit() {
    getchar();
    return 0;
}

int main(int argc, char **argv) {
    int err;
    assert((logfile = fopen("medlog", "w+")) != 0);
    fprintf(logfile, "Starting log\n");
    assert(setup_display() >= 0);
    show_status("Starting");
    LOG(logfile, "At start\n");
    buf_status(&main_buf);
    if (argc > 1) {
        err = buf_open(&main_buf, main_file_name, main_file);
    }
    if (argc == 1 || err) {
        buf_init(&main_buf, 1000);
    }
    LOG(logfile, "After buf_init\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    const char speed[] = "Speed Racer";
    buf_insert(&main_buf, speed, sizeof(speed));
    LOG(logfile, "After insert\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    buf_seek(&main_buf, 5);
    LOG(logfile, "After seek\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    draw_pane();
    edit();
    cleanup_display();
}
