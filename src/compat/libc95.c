// Enough of libc to make this thing compile without MSVCRT.
// Released into the public domain under the CC0-1.0 licence.

#include <windows.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "libc95.h"

void _winstart(void) {
  // ... uhhh, this might be it?
  ExitProcess(WinMain(GetModuleHandleA(NULL), NULL, GetCommandLine(), SW_SHOW));
}

void *memmove(void *dst, const void *src, size_t n) {
  if ((uintptr_t)dst < (uintptr_t)src) {
    // dst < src = copy from start
    size_t i = 0;
    while (i < n) { ((char *)dst)[i] = ((const char *)src)[i]; i += 1; }
  } else if ((uintptr_t)dst > (uintptr_t)src) {
    // dst > src = copy from end
    size_t i = n;
    while (i > 0) { i -= 1; ((char *)dst)[i] = ((const char *)src)[i]; }
  }
  return dst;
}

void *memset(void *dst, int c, size_t len) {
  size_t i = 0;
  while (i < len) { ((char *)dst)[i] = (char)c; i += 1; }
  return dst;
}

char *strcat(char *dst, const char *src) {
  char *target = dst;
  size_t i = 0;
  while (*target != 0) { target += 1; }
  while (src[i] != 0) { target[i] = src[i]; i += 1; }
  target[i] = 0;
  return dst;
}

char *strcpy(char *dst, const char *src) {
  size_t i = 0;
  while (src[i] != 0) { dst[i] = src[i]; i += 1; }
  dst[i] = 0;
  return dst;
}

size_t strlen(const char *s) {
  size_t result = 0;
  while (s[result] != 0) { result += 1; }
  return result;
}

char *strupr(char *s) {
  size_t i = 0;
  while (s[i] != 0) {
    if (s[i] >= 'a' && s[i] <= 'z') { s[i] += 'A' - 'a'; }
    i += 1;
  }
  return s;
}

char *strchr(const char *s, int c) {
  size_t i = 0;
  while (s[i] != 0 && s[i] != (char)c) { i += 1; }
  return (s[i] == (char)c ? (char *)&s[i] : NULL);
}

int strcmp(const char *s1, const char *s2) {
  size_t i = 0;
  while (s1[i] != 0 && s1[i] == s2[i]) { i += 1; }
  return (s1[i] < s2[i] ? -1 : s1[i] > s2[i] ? 1 : 0);
}

long atol(const char *s) {
  // FIXME doesn't care about overflow
  long result = 0;
  size_t i = 0;
  while (s[i] >= '0' && s[i] <= '9') { result = (result*10L)+(long)(s[i]-'0'); i += 1; }
  //{ char msgbuf[100]; sprintf(msgbuf, "result %d", (int)result); MessageBoxA(0, msgbuf, "libc95 debug", 0); }
  return result;
}

static unsigned int rand_seed = 1;
int rand(void) {
  rand_seed = ((rand_seed * 22695477U) + 1U) & 0x7FFFFFFFU;
  return (int)(rand_seed>>16);
}
void srand(unsigned int seed) {
  rand_seed = seed & 0x7FFFFFFFU;
}

int abs(int v) {
  return (v < 0 ? -v : v);
}

time_t time(time_t *tloc) {
  SYSTEMTIME st = {0};
  FILETIME ft = {0};
  // GetSystemTimeAsFileTime on NT requires NT 3.5.
  // This sequence should work on NT 3.1.
  GetSystemTime(&st);
  SystemTimeToFileTime(&st, &ft);
  // Convert 10^-8 to seconds
  uint32_t lo = ft.dwLowDateTime;
  uint32_t hi = ft.dwHighDateTime;
  // Got no (_)_udivdi3() on hand, so I have to do this the hard way.
  // ... without any CPU assistance as I'm not sure how well TinyCC handles inline asm if at all.
  // FIXME THIS IS CURRENTLY UNTESTED
  uint64_t wide = (((uint64_t)hi)<<(uint64_t)32)|(uint64_t)lo;
  uint64_t accum = 0;
  uint64_t den = 10*1000*1000;
  uint64_t adder = 1;
  // Shift it into place
  den <<= (uint64_t)(8+32);
  adder <<= (uint64_t)(8+32);
  for (int i = 0; i <= 40; i++) {
    if (wide >= den) {
      wide -= den;
      accum += adder;
    }
    den >>= (uint64_t)1;
    adder >>= (uint64_t)1;
  }
  time_t t = (time_t)accum;

  if (tloc != NULL) { *tloc = t; }
  return t;
}

static void append_decimal(char **dp, unsigned long v) {
  if (v >= 10) { append_decimal(dp, v/10); }
  *((*dp)++) = '0' + (v%10);
}

// Kye only uses `%d`, so that's all we're supporting right now
int sprintf(char *dst, const char *fmt, ...) {
  char *d = dst;
  const char *s = fmt;
  va_list ap;
  va_start(ap, fmt);

  while (*s != 0) {
    if (*s == '%') {
      s += 1;
      switch (*(s++)) {
        case 'd':
          {
            int v = va_arg(ap, int);
            if (v < 0) {
              *(d++) = '-';
              append_decimal(&d, (unsigned long)(-v));
            } else {
              append_decimal(&d, (unsigned long)v);
            }
          }
          break;

        default:
          // TODO!
          break;
      }
    } else {
      *(d++) = *(s++);
    }
  }

  va_end(ap);
  *d = 0;
  return (int)(d-dst);
}

// I have to remove the wrapper around FILE otherwise TinyCC complains about an incompatible redefinition. Don't worry, we won't actually make any use of the actual structure!
#undef FILE

// Kye uses "r" and "w".
FILE *fopen(const char *path, const char *mode) {
  if (!strcmp(mode, "r")) {
    // Allocate a buffer.
    real_file_t *rf = (real_file_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*rf));
    if (rf == NULL) { return NULL; }
    // Open the file if we can.
    HANDLE h = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!h) {
      (void)HeapFree(GetProcessHeap(), 0, rf);
      return NULL;
    }
    rf->h = h;
    rf->buf_filled = 0;
    rf->buf_read_idx = 0;
    rf->is_at_eof = 0;
    return (FILE *)rf;

  } else if (!strcmp(mode, "w")) {
    // TODO WRITE FILES
    return NULL;

  } else {
    // Not supported (yet?)!
    return NULL;
  }
}

int fclose(FILE *fp) {
  real_file_t *rf = (real_file_t *)fp;
  // I don't care if there's an error.
  (void)CloseHandle(rf->h);
  (void)HeapFree(GetProcessHeap(), 0, rf);
  return 0;
}

void top_up_read_buffer(real_file_t *rf) {
  // If at EOF, don't attempt to fill up further.
  if (rf->is_at_eof) { return; }

  // Move buffer data back so we can read more data at the end
  if (rf->buf_read_idx != 0) {
    memmove(&rf->buf[0], &rf->buf[rf->buf_read_idx], rf->buf_filled - rf->buf_read_idx);
    rf->buf_filled -= rf->buf_read_idx;
    rf->buf_read_idx = 0;
  }

  // If we can read more data, then read more data.
  if ((sizeof(rf->buf) - rf->buf_filled) >= 1) {
    DWORD num_read = 0;
    if (ReadFile(rf->h, &rf->buf[rf->buf_filled], (DWORD)(sizeof(rf->buf) - rf->buf_filled), &num_read, NULL)) {
      rf->buf_filled += (size_t)num_read;
    } else {
      rf->is_at_eof = 1;
    }
  }
}

int ungetc(int c, FILE *fp) {
  real_file_t *rf = (real_file_t *)fp;

  // "Only one push-back is guaranteed."
  //
  // If at start, only push back if we are at the end of the filled part of the buffer and the buffer is not full.
  // Otherwise I might have just broken this guarantee. Oh well.
  // Not sure if this lifeline is necessary.
  if (rf->buf_read_idx < 1) {
    if (rf->buf_filled == sizeof(rf->buf)) {
      rf->buf_filled += 1;
      rf->buf[rf->buf_read_idx] = (char)c;
      return c;
    } else {
      return EOF;
    }
  }

  rf->buf_read_idx -= 1;
  rf->buf[rf->buf_read_idx] = (char)c;
  return c;
}

int fgetc(FILE *fp) {
  real_file_t *rf = (real_file_t *)fp;

  while (1) {
    // Attempt a top-up.
    if ((rf->buf_filled - rf->buf_read_idx) < 1) { top_up_read_buffer(rf); }
    // If we still can't fit it in, we've hit EOF.
    if ((rf->buf_filled - rf->buf_read_idx) < 1) { return EOF; }

    int c = (int)(unsigned char)(rf->buf[rf->buf_read_idx]);
    // KLUDGE: Assume \r is always followed by \n and just strip the \r .
    rf->buf_read_idx += 1;
    if (c == '\r') { continue; }
    return c;
  }
}

char *fgets(char *s, int size, FILE *fp) {
  if (size >= 1) {
    size_t i = 0;
    while (i+1 < (size_t)size) {
      int c = fgetc(fp);
      if (c == EOF) { break; }
      s[i] = c;
      i += 1;
      // The newline is always included.
      if (c == '\n') { break; }
    }
    s[i] = 0;
    if (i == 0) { return NULL; }
  }
  return s;
}

int fputs(const char *s, FILE *fp) {
  real_file_t *rf = (real_file_t *)fp;
  // TODO!
  return -1;
}

int fputc(int c, FILE *fp) {
  real_file_t *rf = (real_file_t *)fp;
  // TODO!
  return -1;
}

int fseek(FILE *fp, long offset, int whence) {
  real_file_t *rf = (real_file_t *)fp;
  switch (whence) {
    // Kye only uses SEEK_SET, so that's the only one that's tested.
    case SEEK_SET:
      if (SetFilePointer(rf->h, (LONG)offset, NULL, FILE_BEGIN) == 0xFFFFFFFF) { return -1; }
      break;
    case SEEK_CUR:
      if (SetFilePointer(rf->h, (LONG)offset, NULL, FILE_CURRENT) == 0xFFFFFFFF) { return -1; }
      break;
    case SEEK_END:
      if (SetFilePointer(rf->h, (LONG)offset, NULL, FILE_END) == 0xFFFFFFFF) { return -1; }
      break;
    default:
      // Invalid!
      return -1;
  }
  // Now clear our buffers and return successfully!
  rf->is_at_eof = 0;
  rf->buf_read_idx = 0;
  rf->buf_filled = 0;
  return 0;
}
