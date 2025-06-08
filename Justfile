alias b := build
alias g := generate

run: build hello-world
    ./build/spaced
    xxd -g2 dump.bin | grep -v '0000 0000 0000 0000 0000 0000 0000 0000'

build: generate
    mkdir -p ./build/
    clang -Isrc src/main.c -o ./build/spaced -std=c99 -g

generate:
    mkdir -p ./src/generated
    python3 ./x.py

hello-world:
    cc65 -t none --cpu 6502 examples/c/hello-world/main.c
    ca65 --cpu 6502 examples/c/hello-world/main.s
    ca65 --cpu 6502 target/vectors.s
    ca65 --cpu 6502 target/interrupt.s
    ld65 -C target/memory.cfg -m examples/c/hello-world/main.map examples/c/hello-world/main.o -o examples/c/hello-world/main.bin target/stdlib.lib target/vectors.o target/interrupt.o
    da65 examples/c/hello-world/main.bin > examples/c/hello-world/out.s -S 0x01000 -v
    xxd -p ./examples/c/hello-world/main.bin
