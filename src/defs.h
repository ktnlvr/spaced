#ifndef __SPACED_H__DEFS__
#define __SPACED_H__DEFS__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PANIC_(msg)                                                            \
  do {                                                                         \
    fprintf(stderr, "%s:%d\t\t" msg "\n", __FILE__, __LINE__);                 \
    abort();                                                                   \
  } while (0)

#define PANIC(msg, ...)                                                        \
  do {                                                                         \
    fprintf(stderr, "%s:%d\t\t" msg "\n", __FILE__, __LINE__, __VA_ARGS__);    \
    abort();                                                                   \
  } while (0)

#define EXPECT__(expr) do { if (!(expr)) { PANIC_(#expr); } } while(0)

#define EXPECT_(expr, msg)                                                     \
  do {                                                                         \
    if (!(expr))                                                               \
      PANIC_("Asserton Failed! " msg);                                         \
  } while (0)

#define EXPECT(expr, msg, ...)                                                 \
  do {                                                                         \
    if (!(expr))                                                               \
      PANIC("Assertion Failed! " msg, __VA_ARGS__);                            \
  } while (0)

#ifndef NDEBUG
#define ASSERT(expr, msg, ...) EXPECT(expr, msg, __VA_ARGS__)
#define ASSERT_(expr, msg) EXPECT_(expr, msg)
#else
#define ASSERT(expr, msg, ...) ;
#define ASSERT_(expr, msg) ;
#endif

// TODO: replace builtins
#define FLOAT32_INFTY (__builtin_inff())
#define FLOAT32_NEG_INFTY (-__builtin_inff())

typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t i32;
typedef uint16_t u16;
typedef unsigned char byte;
typedef char i8;
typedef size_t sz;

#endif
