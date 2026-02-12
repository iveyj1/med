#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include "ansi_escapes.h"
#include "disp.h"

struct winsize ws = {0, 0};

int setup_display() {
    setupConsole();
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        printf("Error getting screen size");
        return -1;
    }
    return 0;
}

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

int cleanup_display() {
    moveTo(ws.ws_row, 0);
    clearLine();
    restoreConsole();
    return 0;
}

int draw_pane() {
    clearScreen();
    moveTo((int)ws.ws_row / 2, (int)ws.ws_col / 2);
    printf("Speed Racer");
    return 0;
}
