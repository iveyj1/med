#ifndef __UTILS_H_
#define __UTILS_H

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
#endif
