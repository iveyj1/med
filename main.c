#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include "utils.h"
#include "disp.h"
#include "buf.h"

#define MAX_FILENAME_LEN 1024

FILE *logfile;

enum mode { NORMAL, INSERT, COMMAND };
struct text_buf main_buf = {0, 0, -1, 0, 0, 0, {0}, 0};

int edit() {
    getchar();
    return 0;
}

#define MAX_STATUS_LENGTH 256
char status[MAX_STATUS_LENGTH] = "";

int main(int argc, char **argv) {
    int  err;
    char file_name[MAX_FILENAME_LEN] = "";
    assert_bt((logfile = fopen("medlog", "w+")) != 0);
    assert_bt(setup_display() >= 0);
    assert_bt(argc > 1);
    err = buf_open(&main_buf, argv[1]);
    snprintf(status, MAX_STATUS_LENGTH, "%s:%d", main_buf.fname, buf_chars(&main_buf));
    draw_pane(&main_buf);
    edit();
    cleanup_display();
    return 0;
}
