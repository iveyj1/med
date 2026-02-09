# Two Reusable Primitives for Safe C String Handling

This document reduces safe string copy and concatenation in **standard C** to **two reusable primitives**.
Everything else is policy layered on top.

No folklore. No convenience wrappers. Just invariants.

---

## Primitive 1: bounded copy with termination

### Contract
- Copies as much of `src` as fits in `dst`
- Always NUL-terminates if `dstsz > 0`
- Returns **full length of `src`**
- Truncation is detectable: `ret >= dstsz`

### Implementation

```c
#include <stddef.h>
#include <string.h>

size_t str_copy_n(char *dst, size_t dstsz, const char *src)
{
    size_t srclen = strlen(src);

    if (dstsz == 0)
        return srclen;

    size_t n = (srclen < dstsz - 1) ? srclen : dstsz - 1;

    memcpy(dst, src, n);
    dst[n] = '\0';

    return srclen;
}
```

### Postconditions
- `dst` is always a valid C string if `dstsz > 0`
- No read or write out of bounds
- Return value fully describes truncation

---

## Primitive 2: bounded append with termination

### Contract
- Appends `src` to `dst`
- Always NUL-terminates if `dstsz > 0`
- Returns **would-be total length** (`strlen(dst₀) + strlen(src)`)
- Truncation detectable: `ret >= dstsz`

### Implementation

```c
#include <stddef.h>
#include <string.h>

size_t str_cat_n(char *dst, size_t dstsz, const char *src)
{
    size_t dstlen = strlen(dst);
    size_t srclen = strlen(src);

    if (dstlen >= dstsz)
        return dstlen + srclen;

    size_t avail = dstsz - dstlen - 1;
    size_t n = (srclen < avail) ? srclen : avail;

    memcpy(dst + dstlen, src, n);
    dst[dstlen + n] = '\0';

    return dstlen + srclen;
}
```

### Postconditions
- `dst` remains a valid C string
- Concatenation is bounded and explicit
- Return value preserves intent

---

## Why these two are sufficient

Every safe string operation reduces to:

| Operation | Reduction |
|---------|-----------|
| Initialize buffer | `str_copy_n` |
| Append | `str_cat_n` |
| Truncating copy | `str_copy_n` |
| Truncating append | `str_cat_n` |
| Format then append | `snprintf` → `str_cat_n` |
| Zero-fill policy | wrap either primitive |

No other string primitive is required.

---

## Canonical usage pattern

```c
char buf[64];

if (str_copy_n(buf, sizeof buf, "hello") >= sizeof buf)
    /* truncated */;

if (str_cat_n(buf, sizeof buf, " world") >= sizeof buf)
    /* truncated */;
```

---

## What this deliberately avoids

- `strncpy`
- `strncat`
- `strcpy`, `strcat`
- implicit termination
- silent truncation

---

## Mental model

**Strings are byte arrays with a sentinel.**  
These two primitives enforce that invariant explicitly.
