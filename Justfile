alias b := build
alias g := generate

run: build hello-world
    ./build/spaced

build: generate
    mkdir -p ./build/
    clang -Isrc src/main.c -o ./build/spaced -std=c99 -g

generate:
    mkdir -p ./src/generated
    python3 ./x.py

hello-world:
    cc65 ./examples/c/hello-world/main.c
    ca65 ./examples/c/hello-world/main.s
    ld65 -t none ./examples/c/hello-world/main.o -o ./examples/c/hello-world/main.bin none.lib
    da65 ./examples/c/hello-world/main.bin > ./examples/c/hello-world/out.s -S 0x01000 -v
    xxd -p ./examples/c/hello-world/main.bin
