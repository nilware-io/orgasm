# nanolang

A visual dataflow programming language with a node-based editor, standalone compiler, and runtime. Programs are authored as flow graphs in `.nano` files and compiled to C++. See an [example program](scenes/klavier/main.nano) and the full [language specification](nanolang.md).

![nanolang](https://github.com/skmp/nanolang/blob/main/docs/nanolang.png)

## Components

| Target | Description |
|--------|-------------|
| **nanolang** | Core library — type system, expression parser, type inference, serialization |
| **nanoflow** | Visual node editor (SDL3 + Dear ImGui) |
| **nanoc** | Standalone compiler (`.nano` → C++) |
| **nanoruntime** | Runtime with GUI and ImGui bindings |

## Language Highlights

- **Sigil-based value categories** — `%` data, `&` reference, `^` iterator, `@` lambda, `#` enum, `!` bang (trigger), `~` event
- **Rich type system** — scalars (`u8`–`s64`, `f32`/`f64`, `bool`, `string`), containers (`vector`, `map`, `list`, `set`, `queue`), fixed-size arrays, tensors, named struct types, function types
- **Bidirectional type inference** with automatic integer upcasting and iterator-to-reference decay
- **Bang-driven control flow** — nodes postfixed with `!` have explicit execution ordering via bang signals
- **Inline expressions** — node arguments can embed literals, variable refs, and sub-expressions directly
- **Lambda construction** — expression nodes can be captured as callable lambdas with automatic capture/parameter resolution
- **FFI support** — declare external C functions and call them from the graph
- **Standard library modules** — e.g. `decl_import std/imgui` for ImGui bindings

## Building

Requires CMake 3.25+ and a C++20 compiler. SDL3 and Dear ImGui are fetched automatically on Linux/macOS; on Windows they are managed via [vcpkg](https://vcpkg.io).

### Linux

```bash
# install SDL3 build dependencies
sudo apt-get install libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxfixes-dev libxss-dev libxtst-dev libwayland-dev libxkbcommon-dev libegl-dev libgles-dev

cmake -B build && cmake --build build --parallel
```

### Windows

```bash
# install vcpkg if you haven't already
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.bat

cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --parallel --config Release
```

## License

MIT — see [LICENSE](LICENSE).
