#if !defined(COMMON_H)
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define INTERNAL static
#define PERSIST static

#define FALLTHROUGH_INTENDED
#define NO_DEFAULT_CASE

#define TO_STRING(val) #val
#define STRINGIFY(val) TO_STRING(val)

#define COMPILER "clang-"STRINGIFY(__clang_major__)"."\
  STRINGIFY(__clang_minor__)"."STRINGIFY(__clang_patch_level__)

#define PURE __attribute__((pure))
#define CONST __attribute__((const))
#define HOT __attribute__((hot))
#define COLD __attribute__((cold))
#define CHECK __attribute__((warn_unused_result))
#define DEPRECATED(msg) __attribute__((deprecated(msg)))
#define UNUSED __attribute__((unused))
#define LIKELY(val) __builtin_expect(!!(val), 1)
#define UNLIKELY(val) __builtin_expect(!!(val), 0)
#define DESIGNATED __attribute__((designated_init))
#define RAW_MEM __attribute__((malloc))
#define NO_NULL_RET __attribute__((returns_nonnull))
#define NO_RETURN __attribute__((noreturn))
#define CLEANUP(fnc) __attribute__((__cleanup__(fnc)))
#define PRINTF_FMT(fmt_str_index, args_index) \
  __attribute__((format(printf, fmt_str_index, args_index)))
#define NOT_NULL _Nonnull

typedef unsigned int uint;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintmax_t umax;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef char byte;

#define FATAL(...) \
  fatal(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
PRINTF_FMT(4, 5) NO_RETURN void 
fatal(
  char const *const file_name, 
  int const line_num, 
  char const *const fnc_sig, 
  char const *const restrict fmt_str, 
  ...
)
{
  va_list args;
  va_start(args, fmt_str);

  fprintf(stderr, "%s:%d:%s\n[RYLANG FATAL]: ", file_name, line_num, fnc_sig);
  vfprintf(stderr, fmt_str, args);
  fprintf(stderr, "\n");
  fflush(stderr);

  exit(EXIT_FAILURE);
}

#define MIN(val1, val2) \
  ({ __auto_type _val1 = val1; \
     __auto_type _val2 = val2; \
	 __builtin_types_compatible_p(typeof(_val1), typeof(_val2)) ? \
	   (_val1 < _val2 ? _val1 : _val2) : fatal("Attempting to calculate min between mismatched types\n"); \
  }) 

#define MAX(val1, val2) \
  ({ __auto_type _val1 = val1; \
     __auto_type _val2 = val2; \
	 __builtin_types_compatible_p(typeof(_val1), typeof(_val2)) ? \
	   (_val1 > _val2 ? _val1 : _val2) : fatal("Attempting to calculate max between mismatched types\n"); \
  }) 

#if !defined(NDEBUG)
typedef struct {
  umax id;
} TimedBlock;
typedef struct {
  clock_t time; 	
  char const *const file_name;
  char const *const function_name;
  umax const line_num;
  size_t const hit_count;
} TimedBlockInfo;

TimedBlock* global_timed_blocks = buf_init(sizeof(TimedBlock));

inline void end_timed_block(TimedBlock* timed_block)
{
  global_timed_blocks[timed_block.id].time = 
}

#define TIMED_BLOCK \
  umax _cur_counter = __COUNTER__; \
  _cur_counter < buf_len(global_timed_blocks) ? global_timed_blocks[_cur_counter] = {} : buf_push()
  TimedBlock timed_block##__LINE__ = {\
    .id = cur_counter; \
  } CLEANUP(end_timed_block);
#else
#define TIMED_BLOCK
#endif

RAW_MEM NO_NULL_RET void*
xmalloc(size_t const num_bytes)
{
  void* mem = malloc(num_bytes);
  if (mem == NULL) {
    fatal(
	  "Failed to allocate %zd bytes.\nErrno %d: %s", 
	  num_bytes, 
	  errno, 
	  strerror(errno)
	);
  } else {
    return mem;	  
  }
}

RAW_MEM NO_NULL_RET void*
xcalloc(size_t const num_items, size_t const item_size)
{
  void* mem = calloc(num_bytes);
  if (mem == NULL) {
    fatal(
	  "Failed to allocate %zd items of size %zd.\nErrno %d: %s", 
	  num_items,
	  item_size, 
	  errno, 
	  strerror(errno)
	);
  } else {
    return mem;	  
  }
}

NO_NULL_RET void*
xrealloc(void *const restrict orig_mem, size_t mem_size_change)
{
  void* mem = realloc(orig_mem, mem_size_change);
  if (mem == NULL) {
    fatal(
	  "Failed to reallocate %zd bytes.\nErrno %d: %s", 
	  mem_size_change,
	  errno, 
	  strerror(errno)
	);
  } else {
    return mem;	  
  }
}

typedef struct {
  size_t cap;
  size_t len;
  size_t const elem_size;
  byte const content[];
} Buf;

void*
buf_init(size_t const elem_size)
{
  Buf* buf = xmalloc(size);
  buf->cap = 0;
  buf->len= 0;
  buf->elem_size = elem_size;

  return buf->content;
}

inline Buf*
buf__header(void *const restrict content)
{
  return (Buf* )((byte* )content - offsetof(Buf, content));
}

inline size_t
buf_cap(void *const restrict content)
{
  return buf__header(content)->cap;
}

inline size_t
buf_len(void *const restrict content)
{
  return buf__header(content)->len;
}

inline size_t
buf_elem_size(void *const restrict content)
{
  return buf__header(content)->elem_size;
}

void
buf_fit()
{
  amount <= buf_cap(content) ? NOP : content = buf__grow(content, amount);	
}

void
buf_push()
{
  buf_fit(content, buf_len(content) + 1), content[buf__header(content)->len++] = elem;	
}

void buf_grow()
{
  size_t new_cap = CLAMP_MIN(2 * buf_cap(content), MAX(new_len, 16));	
  size_t new_size = offsetof(Buf, content) + new_cap * buf_elem_size(content);
  Buf* new_buf = xrealloc(buf__header(content), new_size);	  
  new_buf->cap = new_cap;
  return new_buf->content;
}

inline void
buf_free(void *const restrict content)
{
  free(buf__header(content));
}

/*
file io
*/

#endif
