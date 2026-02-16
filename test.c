

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
    log_msg(logfile, "After seek to %d\n", a);
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
