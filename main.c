#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include "utils.h"
#include "disp.h"
#include "buf.h"

FILE *logfile;
FILE *main_file            = 0;
char  main_file_name[1024] = "";
enum mode { NORMAL, INSERT, COMMAND };
struct text_buf main_buf = {0, 0, -1, 0, 0, 0};

int edit() {
    getchar();
    return 0;
}

int main(int argc, char **argv) {
    printf("starting med\n");
    fflush(NULL);
    getchar();
    int err;
    assert((logfile = fopen("medlog", "w+")) != 0);
    fprintf(logfile, "Starting log\n");
    assert(setup_display() >= 0);
    show_status("Starting");
    LOG(logfile, "At start\n");
    buf_status(&main_buf);
    // if (argc > 1) {
    //     err = buf_open(&main_buf, main_file_name, main_file);
    // }
    if (argc == 1 || err) {
        buf_init(&main_buf, 256);
    }
    LOG(logfile, "After buf_init\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    const char speed[] = "Speed Racer";
    buf_append(&main_buf, speed, sizeof(speed));
    LOG(logfile, "After append\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    buf_seek(&main_buf, 5);
    LOG(logfile, "After seek to 5\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    buf_seek(&main_buf, 8);
    LOG(logfile, "After seek to 8\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    buf_seek(&main_buf, 0);
    LOG(logfile, "After seek to 0\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    buf_seek(&main_buf, 6);
    LOG(logfile, "After seek to 6\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    buf_append(&main_buf, "append", sizeof("append"));
    LOG(logfile, "After append\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    buf_seek(&main_buf, 0);
    LOG(logfile, "After seek to 0\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    buf_append(&main_buf, "append", sizeof("append"));
    LOG(logfile, "After append\n");
    buf_status(&main_buf);
    buf_dump(&main_buf);
    draw_pane(&main_buf);
    edit();
    cleanup_display();
}
