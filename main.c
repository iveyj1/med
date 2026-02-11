#include <stdio.h>
#include "ansi_escapes.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "utils.h"

struct text_buf {
    char  *gap_buf;
    size_t gap_buf_len;
    int    cursor;
    int    back;
    int    dirty;
    int    target_gap_len;
};

struct text_buf main_buf = {0, 0, 0, 0, 0};

FILE *main_file            = 0;
char  main_file_name[1024] = "";
enum mode { NORMAL, INSERT, COMMAND };
struct winsize ws = {0, 0};

int show_status(const char *fmt, ...) {
    int row, col;
    getCursorPosition(&row, &col);
    moveTo(ws.ws_row - 1, 0);
    clearScreenToBottom();
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    moveTo(row, col);
    return (0);
}

int insert(struct text_buf *buf, char *str, size_t maxstrlen) {
    if (buf == 0) {
        return -1;
    }
    int len = str_len(str, maxstrlen);
    if (len >= buf->back - buf->cursor) {
        // grow buffer
        return -1;
    }
    memcpy(buf->gap_buf + buf->cursor, str, len);
    buf->cursor += len;
    return 0;
}

// fills a buffer with one line from the text buffer starting from startindex
int buffer_getline(struct text_buf *buf, int startindex, char *line, int linebuflen) {
    if (buf == 0) {
        show_status("no buffer in buffer_getline");
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

int viewport_top_line;

int draw_pane() {
    clearScreen();
    moveTo((int)ws.ws_row / 2, (int)ws.ws_col / 2);
    printf("Speed Racer");
    return 0;
}

size_t get_file_size(FILE *file) {
    if (file == 0) {
        show_status("no file in get_file_size");
        return (-1);
    }
    size_t size, pos;
    pos = ftell(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, pos, SEEK_SET);
    return (size);
}

int init_buf(struct text_buf *buf, int text_size) { 
    if(buf == 0) {
        show_status("no buffer in init_buf");
        return -1;
    }
    if(buf->gap_buf != 0) {
        show_status("Error: buf has non-zero buffer pointer in init_buf");
        return -1;
    }
    buf->target_gap_len = 65536;
    buf->gap_buf_len = text_size + buf->target_gap_len;
    buf->gap_buf = calloc(buf->gap_buf_len, sizeof(char));
    buf->cursor = 0;
    buf->back = buf->gap_buf_len - 1;
    buf->dirty = false;
    return 0; 
}

int open_buf(struct text_buf *buf, const char *filename, FILE *file) { 
    size_t size; 
    if(buf == 0) {
        show_status("open_buf: no buffer");
        return -1;
    }
    if(strlen(filename) > 0) {
        file = fopen(filename, "rw+");
        if(file == 0) {
            show_status("open_buf: can't open %s", filename);
            return -1;
        }
        size = get_file_size(file);
    } else {
        size = 65536;
    }
    if(init_buf(&main_buf, size) < 0) {
        show_status("open_buf: could not initialize buffer");
        return -1;
    }

#ifdef ASSERTIONS 
    assert(size == buf->cursor + buf->gap_buf_len - buf->back);
#endif
    return size; 
}

int setup_display() {
    setupConsole();
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        printf("Error getting screen size");
        return -1;
    }
    return 0;
} 

int edit() {
    getchar();
    return 0;
}

FILE logfile;

#define LOG(fmt, ...) fprintf(logfile, fmt, __VA_ARGS__)

int main(int argc, char **argv) {
    int ret;
    logfile = fopen("medlog", "w+");
    if(logfile == 0) {
        printf("couldn't open medlog for write");
        return -1;
    }
    if(setup_display() == -1) {
        log("Couldn't set up display", LOG_ERR, LOG_STATUS);
        return -1;
    }
    show_status("starting");
    if(argc > 1){
        ret = open_buf(&main_buf, main_file_name, main_file);
    } 
    if(argc == 1 || ret != 0) {
        init_buf(&main_buf, 100000);
    }
    for(;;) {
        draw_pane();
        if(edit() != 0) {
            break;
        }
    }
    moveTo(ws.ws_row, 0);
    clearLine();
    restoreConsole();
}
