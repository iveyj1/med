
#ifndef __BUF_H__
#define __BUF_H__
#include <stddef.h>
#include <stdio.h>

// gap buf - char array on heap allocated at init time
// gap_buf_len - number of chars in above
// cursor - index into gap_buf array of last character of valid
//     content at the start of the buffer
// back - index into gap_buf array of first valid character at the end of array
// target_gap_len - when resizing buffer, this is the target of scratch
//     area between the front and back array
// fp - FILE pointer of file associated with buffer
// fname - file name associated with buffer
// dirty - flag indicating buffer contents have changed since last written
// 
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
int buf_seek_line(struct text_buf *buf, int numlines);
inline int buf_tell(struct text_buf *buf) { return buf->cursor };
int buf_open(struct text_buf *buf, const char *filename);
int buf_get_char(struct text_buf *buf, int index);


#endif
