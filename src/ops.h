#ifndef __SPACED_H__OPS__
#define __SPACED_H__OPS__

#include "defs.h"

typedef enum op_register_t {
  // General purpose register
  R_X = 0b000,
  R_Y = 0b001,
  // Access register, for location to be accessed
  R_AR_LO = 0b010,
  R_AR_HI = 0b011,
  // Accumulator for arithmetic
  R_ACC = 0b100,
  // Flag register for the data about instruction results
  R_F = 0b101,

  // Program counter
  R_PC_LO = 0b110,
  R_PC_HI = 0b111
} op_register_t;

static const char *op_register_to_str(op_register_t r) {
  const char *table[] = {
      "X", "Y", "AL", "AH", "A", "F", "PL", "PH",
  };

  if (r > 7)
    return 0;
  else
    return table[r];
}

typedef enum op_t {
  OP_NOOP = 0b00000000,
#define _OP_NOOP() OP_NOOP
  OP_BREAK = 0b00000001,
  // A <- M[PC++]
  OP_CONST = 0b00000010,
#define _OP_CONST()

  // a --
  OP_POP = 0b00000011,
  // -- PL PH
  OP_PUSH_PC = 0b00000100,
  // a b --, PL <- a, PH <- b
  OP_PULL_PC = 0b00000101,

  // `0b00 010 XXX`: X <- M[AR]
  OP_READ = 0b00010000,
  // `0b00 011 XXX`: M[AR] <- X
  OP_WRITE = 0b00011000,

  // `0b00 100 XXX`: -- X
  OP_PUSH = 0b00100000,
  // `0b00 101 XXX`: X --
  OP_PULL = 0b00101000,

  // `0b00 110 XXX`: if X = 0 then PC <- AR else NOOP
  OP_JMPZ = 0b00110000,
#define _OP_JMPZ(R) (OP_JMPZ | ((R)&0b111))
  // `0b00 111 XXX`: if X = 0 then PC <- M[AR] else NOOP
  OP_JMPZI = 0b00111000,

  // `0b01 000 XXX`: A <- A + X
  OP_ADD = 0b01000000,
#define _OP_ADD(R) (OP_ADD | ((R)&0b111))

  // `0b01 001 XXX`: A <- A - X
  OP_SUB,
  // `0b01 010 XXX`: A <- A * X
  OP_MUL,
  // `0b01 011 XXX`: A <- A / X
  OP_DIV,
  // `0b01 100 XXX`: A <- A xor X
  OP_XOR,
  // `0b01 101 XXX`: A <- A & X
  OP_AND,
  // `0b01 110 XXX`: A <- A | X
  OP_OR,
  // `0b01 111 XXX`: A <- A < X
  OP_LT,

  // `0b10 XXX YYY`: Swap data between two places `X, Y <- Y, X`
  OP_SWAP = 0b10000000,
#define _OP_SWAP(A, B) (OP_SWAP | (A << 3) | (B))

  // `0b11 XXX YYY`: Copy data from one place to another `X <- Y`
  OP_COPY = 0b11000000,
#define _OP_COPY(DST, SRC) (OP_COPY | (DST << 3) | (SRC))
} op_t;

static const char *op_to_str(op_t o) {
  byte msbs = (o & 0b11000000) >> 6;
  byte x = (o & 0b00111000) >>> 3;

  if (msbs == 0b11)
    return "COPY";
  else if (msbs == 0b10)
    return "SWAP";
  else if (msbs == 0b00) {
    if (x == 0b000) {
      switch (o) {
      case OP_NOOP:
        return "NOOP";
      case OP_BREAK:
        return "BREAK";
      case OP_CONST:
        return "CONST";
      case OP_POP:
        return "POP";
      case OP_PUSH_PC:
        return "PUSHPC";
      case OP_PULL_PC:
        return "PULLPC";
      default:
        break;
      };
    }

    switch (x) {
    case 0b001:
      return 0;
    case 0b010:
      return "READ";
    case 0b011:
      return "WRITE";
    case 0b100:
      return "PUSH";
    case 0b101:
      return "PULL";
    case 0b110:
      return "JMPZ";
    }
  }
}

#endif
