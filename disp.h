#ifndef __DISP_H__
#define __DISP_H__

struct text_buf;

extern struct winsize ws;

int setup_display(void);

int show_status();

int cleanup_display(void);

int draw_pane(struct text_buf *buf);

extern char status[];

#endif
