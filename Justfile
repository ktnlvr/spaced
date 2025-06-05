alias b := build

build:
    mkdir -p ./build/
    clang -Isrc src/main.c -o ./build/spaced -std=c99 -g

fib: build
    python ./extra/assembler/asm.py examples/asm/fib.txt
    ./build/spaced
