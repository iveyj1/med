#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include "ansi_escapes.h"
#include "buf.h"
#include "disp.h"
#include "utils.h"

struct winsize ws = {0, 0};
struct panepos ppos = {0, 0, 0};

int top_line=0;

int setup_display() {
    setupConsole();
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        printf("Error getting screen size");
        return -1;
    }
    return 0;
}

int set_line(struct text_buf *buf, int line){
    assert(line > 0);
    int lastline = buf_lines(buf);
    if(buf_lines() > lines){
        top_line = line;
    } else {
        top_line = lastline;
    }
    return line;
}

int find_line(struct text_buf *buf, int lines) {

    
    return 0;
}



int page_down(struct text_buf *buf, int pages) {
    int i;
    for(i=1; i<pages; i++) {
        top_line = top_line + ws.ws_row;
        col = find_col(
        find_line(buf, ws.ws_row); }
}



int show_status() {
    int row, col;
    getCursorPosition(&row, &col);
    moveTo(ws.ws_row, 0);
    clearScreenToBottom();
    printf("%s", status);
    moveTo(row, col);
    return (0);
}

int cleanup_display() {
    moveTo(ws.ws_row, 0);
    clearLine();
    restoreConsole();
    return 0;
}

int draw_pane(struct text_buf *buf) {
    clearScreen();
    fflush(NULL);
    moveTo(0, 0);
    fflush(NULL);
    int  line = 1;
    char c;
    for (int i = 0; i < buf_chars(buf); i++) {
        c = buf_get_char(buf, i);
        // log_msg(logfile, "in draw_pane: index %d  char %c %x\n", i, c, c);
        if (c != '\n') {
            putchar(c);
            fflush(NULL);
        } else {
            line++;
            if (line >= ws.ws_row - 1) {
                LOG(logfile, "end of pane reached\n");
                break;
            }
            // LOG(logfile, "newline, moving to left of line %d\n", line);
            moveTo(line, 0);
            fflush(NULL);
        }
    }
    show_status();
    return 0;
}
