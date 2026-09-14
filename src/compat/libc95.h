// Enough of libc to make this thing compile without MSVCRT. - The header file!
// Released into the public domain under the CC0-1.0 licence.

#ifdef RAND_MAX
#undef RAND_MAX
#endif
#define RAND_MAX (1<<15)

#define FILE_BUF_SZ 1024
typedef struct real_file {
  HANDLE h;
  char buf[FILE_BUF_SZ];
  size_t buf_filled;
  size_t buf_read_idx;
} real_file_t;
#define FILE real_file_t
