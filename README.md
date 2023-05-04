# memory-reader

Yet another memory reader in C++20 (or newer).

This project is under consideration.

## Features

Please note not all features below have been implemented, since this project is under development — check it out later.

- Easy to use in both casual applications and complicated scenarios.

  The mechanism is to be designed.

- Fast and robust.

  The mechanism is to be designed.

- Cross-platform.

  memory-reader uses CMake to organize its source codes, and is designed to work with multiple operating systems. Currently it supports Windows and Linux.

## Motivation

It has been 2 years since the debut of my project [osu-memory](https://github.com/UnnamedOrange/osu-memory). Its simplicity, reliability, and performance have yet to be tested, while its project management has already failed to meet the needs of modern software development. Worse, I am still confronted with software crisis, desperately longing for a sample project that can be safely used in the real world.

I hope this project can help me attain:

- Methodology of cross-platform library development using software engineering.
- Experience in designing high-performance and robust programs.
- First step towards the next major release of osu-memory, osu-kps, and...

## Using memory-reader

In your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
  memory-reader
  GIT_REPOSITORY https://github.com/UnnamedOrange/memory-reader
  GIT_TAG v0.3.0 # Check the release page for latest version.
)
FetchContent_MakeAvailable(memory-reader)

# ...

target_link_libraries(your-target-name PRIVATE memory-reader)
```

In your source:

```cpp
#include <memory-reader/all.h>
using namespace orange::memory_reader;
```

## License

Copyright (c) UnnamedOrange. Licensed under the MIT License.

See the [LICENSE](./LICENSE) file in the repository root for full license text.
