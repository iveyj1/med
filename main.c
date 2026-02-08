#include <stdio.h>
#include "ansi_escapes.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

struct text_buf {
    char  *gap_buf;
    size_t len;
    int    cursor;
    int    back;
    int    dirty;
};
struct text_buf main_buf = {0, 0, 0, 0, 0};

FILE *main_file            = 0;
char  main_file_name[1024] = "";
enum mode { NORMAL, INSERT, COMMAND };
struct winsize ws = {0, 0};

int snstrlen(const char *str, int buflen) {
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

int show_status(const char *fmt, ...) {
    int row, col;
    getCursorPosition(&row, &col);
    moveTo(ws.ws_row, 0);
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
    int len = snstrlen(str, maxstrlen);
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
        if (line[i] == "\n") {
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

int init_buf(struct text_buf *buf) { return (0); }

int fill_buf(struct text_buf *buf, FILE *file, size_t where, size_t numchars) { return (0); }

static size_t file_size = 0;

int main(int argc, char **argv) {
    setupConsole();
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        printf("Error getting screen size");
        return -1;
    }
    draw_pane();
    // show_status("speed racer shown");
    // getchar();
    // init_buf(&main_buf);
    // if(argc > 0){
    //     mainfile  = fopen(argv[1]);
    //     if(mainfile == 0) {
    //         status("Error, could not open %s", argv[1]);
    //         return(1);
    //     }
    // }
}
