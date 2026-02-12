#ifndef __UTILS_H_
#define __UTILS_H

#include <stddef.h>
#include <stdio.h>

/**
 * @brief Concatenate two strings, correctly null-terminated concatenated
 * string is in dst
 *
 * @param[in] dest Null-terminated string with first part of string and space
 * second string
 *
 * @param[in] src Null-terminated string
 *
 * @retval String size
 * @retval < dstsz means string was concatenated successfully
 * @retval >= dxtsx means string was truncated
 */
size_t str_cat_n(char *dst, size_t dstsz, const char *src);

/**
 * @brief Copy a string, always leaving a valid null-terminated string in dst
 *
 * @param[in] dest space for string plus null
 *
 * @param[in] src Null-terminated string
 *
 * @retval String size
 * @retval < dstsz means string was copied successfully
 * @retval >= dxtsx means string was truncated
 */
size_t str_copy_n(char *dst, size_t dstsz, const char *src);

/**
 * @brief Get null-terminated string length
 *
 * @param[in] str Null-terminated string
 *
 * @param[in] buflen Length of buffer containing string
 *
 * @retval String size
 */
int str_len(const char *str, int buflen);

/**
 * @brief Get file size
 *
 * @param[in] file FILE pointer
 *
 * @retval File size or -1 on error
 */
size_t get_file_size(FILE *file);

#define LOG(logfile, fmt, ...)            \
    fprintf(logfile, fmt, ##__VA_ARGS__); \
    fflush(logfile);

extern FILE *logfile;

#define CUSTOM_ASSERT(expr)                                                                 \
    do {                                                                                    \
        if (!(expr)) {                                                                      \
            fprintf(stderr, "Assertion failed: %s, at %s:%d\n", #expr, __FILE__, __LINE__); \
            LOG(logfile, "Assertion failed: %s, at %s:%d\n", #expr, __FILE__, __LINE__);    \
            exit();                                                                         \
        }                                                                                   \
    } while (0)

#endif
