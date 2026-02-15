#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
// #include <assert.h>
#include "utils.h"
#include "disp.h"
#include "buf.h"

#define MAX_FILENAME_LEN 1024

FILE *logfile;
FILE *main_file                        = 0;
char  main_file_name[MAX_FILENAME_LEN] = "";
enum mode { NORMAL, INSERT, COMMAND };
struct text_buf main_buf = {0, 0, -1, 0, 0, 0, {0}, 0};

int edit() {
    getchar();
    return 0;
}

// #define TEST_PAUSE_ENABLE
#define TEST_FLUSH_ENABLE

#ifdef TEST_PAUSE_ENABLE
#define TEST_PAUSE() getchar()
#else
#define TEST_PAUSE()
#endif

#ifdef TEST_FLUSH_ENABLE
#define TEST_FLUSH() fflush(NULL)
#else
#define TEST_FLUSH()
#endif

int buf_test_seek(int a) {
    buf_seek(&main_buf, a);
    LOG(logfile, "After seek to %d\n", a);
    buf_status(&main_buf);
    buf_dump(&main_buf);
    draw_pane(&main_buf);
    TEST_FLUSH();
    TEST_PAUSE();
    return 0;
}

int buf_test_append(const char *a, size_t size) {
    buf_append(&main_buf, a, size);
    LOG(logfile, "After append of %s\n", (a));
    buf_status(&main_buf);
    buf_dump(&main_buf);
    draw_pane(&main_buf);
    TEST_FLUSH();
    TEST_PAUSE();
    return 0;
}

int buf_test(const char *a) {
    LOG(logfile, "After %s\n", a);
    buf_status(&main_buf);
    buf_dump(&main_buf);
    draw_pane(&main_buf);
    TEST_FLUSH();
    TEST_PAUSE();
    return 0;
}

int test() {
    const char speed[] = "Speed Racer\n";
    buf_init(&main_buf, 256);
    buf_append(&main_buf, speed, sizeof(speed));
    buf_test("buf append");
    draw_pane(&main_buf);
    getchar();
    buf_seek(&main_buf, 6);
    buf_test("buf seek");
    draw_pane(&main_buf);
    getchar();
    const char asdf[] = ".....";
    buf_append(&main_buf, speed, sizeof(speed));
    buf_test("buf append");
    draw_pane(&main_buf);
    return 0;
}

int main(int argc, char **argv) {
    int err;
    assert_bt((logfile = fopen("medlog", "w+")) != 0);
    fprintf(logfile, "Starting log\n");
    assert_bt(setup_display() >= 0);
    show_status("Starting");
    // buf_status(&main_buf);
    if (argc > 1) {
        str_copy_n(main_file_name, MAX_FILENAME_LEN, argv[1]);
        show_status("Openiing %s", main_file_name);
        err = buf_open(&main_buf, main_file_name, main_file);
    }
    if (argc == 1 || err) {
        buf_init(&main_buf, 256);
    }
    edit();
    cleanup_display();
    return 0;
}
