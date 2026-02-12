extern struct winsize ws;

int setup_display(void);

int show_status(const char *fmt, ...);

int cleanup_display(void);

int draw_pane(void);
