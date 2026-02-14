
#ifndef __BUF_H__
#define __BUF_H__
#include <stddef.h>
#include <stdio.h>

struct text_buf {
    char  *gap_buf;
    size_t gap_buf_len;
    int    cursor;
    int    back;
    int    target_gap_len;
    FILE  *fp;
    char   fname[1024];
    int    dirty;
};

int buf_chars(struct text_buf *buf);
int buf_gap_len(struct text_buf *buf);
int buf_resize(struct text_buf *buf, size_t len);
int buf_init(struct text_buf *buf, int text_size);
int buf_status(struct text_buf *buf);
int buf_dump(struct text_buf *buf);
int buf_append(struct text_buf *buf, const char *str, size_t maxstrlen);
int buf_seek(struct text_buf *buf, int pos);
// int buf_getline(struct text_buf *buf, int startindex, char *line, int linebuflen);
// int buf_open(struct text_buf *buf, const char *filename, FILE *file);
int buf_get_char(struct text_buf *buf, int index);
//
#endif
