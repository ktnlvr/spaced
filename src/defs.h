#ifndef __SPACED_H__DEFS__
#define __SPACED_H__DEFS__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PANIC_(msg)                                                            \
  do {                                                                         \
    printf("ERR! %s:%d\t\t" msg "\n", __FILE__, __LINE__);                     \
    abort();                                                                   \
  } while (0)

#define PANIC(msg, ...)                                                        \
  do {                                                                         \
    printf("ERR! %s:%d\t\t" msg "\n", __FILE__, __LINE__, __VA_ARGS__);        \
    abort();                                                                   \
  } while (0)

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
