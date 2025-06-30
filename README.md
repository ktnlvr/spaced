# subastra

![website](https://badges.cucumberstudio.com/website?url=https%3A%2F%2Fsubastra.ktnlvr.dev)

## The Pitch

## Getting Started

## How it Works

## Testing

## Zen

Philosophical decisions. All code should align with them without exceptions.

1. **If recovery is impossible a crash is warranted.** If a specific error is not possible to recover from in a given context the program should crash with appropriate message.
2. **All sad paths should be signposted.** For every function that can crash it should have error states clearly defined (e.g. returning a null pointer). Alternatively, it should supply all precautions 
3. **OOM is not a recoverable error.** If the program runs out of memory it can only crash. The memory is meticolously managed so running out of memory is either reaching the machine's limitations or a programmer error.
4. **Trivially terminating loops.** All loops should have a *trivial, reachable and proveable* termiation condition. If the loop should, ideally, run forever (i.e. hash chaining), it should have a maximum iteration limit. 
5. **No stack recursion.** A function should never recurse, partially because then its more difficult to verify.
6. **Free memory is poisoned.** The memory handled by an allocator before being given to the program should always be poisoed. This ensures that only the memory that is intended to be used by the program is actually used.

## Conventions

The code and the documentation both use a lot of specific conventions. The project is written in C and is doing its best to avoid memory leaks, buffer overflows and so on.

- The code is riddled with `ASSERT`s. That's good.
- Everything is `snake_case`, unless its trying to mimic another notation. For instance, `glMessageCallback` is trying to copy OpenGL, so its an exception.
- All structure types are postfixed with `_t` except algebraic ones (`vec2`, `vec2i`, `mat4`, etc).
- Callback types are postfixed with `_f` instead of `_t`.
- All member functions of a type are prefixed with the name without a postfix, like `scheduler_plan` for `scheduler_t` and `vec2_new` for `vec2`.
- For a `T` there can exist constructors `T T_new(...)`, `void T_init(T* self, ...)` or both.
- `T_new` does not allocate dynamic resources, `T_init` does.
- If a type manages dynamic resources it will have a destructor `void T_cleanup(T* self)`.
- If a type is a sum-type (i.e. union of structs with an enum identifier), the `_new` family will expose all possible kinds as their own constructors (`T_new_foo`, `T_new_bar` and so on).
- Double underscores in method names usually mean "private".
- Single underscores in the beginning of a field name also mean "private".
- In the context of the `O(N)` notation, `N` and `M` are big constants with `M ≤ N` if applicable.
- Similarly, `n` and `m` are small constants relative to `N` and `M`.
- `//` are a functionality comment, `///` are documenting comments.

## Credits

Coded by Artur Roos. Available on [GitHub](https://github.com/ktnlvr/subastra). What follows is a list of technologies that are important to this project in no particular order.

- [OpenGL](https://www.opengl.org/), the rendering API.
- [cc65 Toolchain](https://cc65.github.io/), a full-on compiler/assembler/linker for the 6502.
- [Doxygen](https://www.doxygen.nl/index.html), a documentation generation tool.
- [Markdown](https://www.markdownguide.org/basic-syntax/), the fanstastic human-readable markup language.
- [Just](https://github.com/casey/just), a Make-like quick command runner.
- [Python](https://www.python.org/), the tool more complex scripting.
- [Git](https://git-scm.com/downloads), the version control system.
- [Aseprite](https://www.aseprite.org/), the best pixel art editor ever.
- [Clang](https://clang.llvm.org/), the best C compiler under the sun.
- [`xxd`](https://linux.die.net/man/1/xxd), the hexdump utility.
- [`ripgrep`](https://github.com/BurntSushi/ripgrep), a faster `grep`.
- [`stb_image.h`](https://github.com/nothings/stb), a header-only public domain library for reading image data.

The following is a looser list of things that were inspirational to the technical aspects of the project.

- [Factorio](https://www.factorio.com/), a game about building the best factories.
- [Screeps.io](https://screeps.com/), a JavaScript game where you control small agents with code.

## License

This project is distributed under the strong copyleft `AGPL-3.0` license. If you make something using this project it also must be AGPL-3.0, kinda. Not a lawyer, this is not legal advice.

## Contributing

## Contact
