#ifndef __SPACED_H__DEFS__
#define __SPACED_H__DEFS__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define PANIC()                                                                \
  do {                                                                         \
    printf("ERR! %s:%d\n", __FILE__, __LINE__);                                \
  } while (0)

typedef uint32_t u32;
typedef int32_t i32;
typedef uint16_t u16;
typedef unsigned char byte;

#endif
