# Matching Engine

A C++17 implementation of a matching engine with order book, TCP networking, and protocol serialization.  
This project builds a reusable library and provides both a server (`matching_engine`) and a client (
`matching_engine_client`) executable.

---

## Features

- **Core library (`matching_engine_lib`)**
    - Order book management
    - Protocol serialization/deserialization
    - TCP client/server components
- **Executables**
    - `matching_engine` — main server binary
    - `matching_engine_client` — client binary
- **Testing**
    - Unit tests using [GoogleTest](https://github.com/google/googletest)
- **Dependencies**
    - [Boost](https://www.boost.org/) (components: `log`, `log_setup`, `system`, `thread`)

---

## Requirements

- CMake **3.25+**
- C++17 compiler (GCC, Clang, or MSVC)
- Boost (with required components)

---

## Build Instructions

### 1. Configure the project

Always use an out-of-source build:

```bash
mkdir build
cd build
cmake ..
```

#### Optional configuration:

- Build type (default: Release):

 ```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

- Ring buffer capacity (default: 4096):
```bash
cmake -DRINGBUFFER_CAPACITY=8192 ..
```

### 2. Build
```bash
cmake --build .
```

Binaries will be placed under:
```text
build/bin/
```
Libraries will be placed under:
```text
build/lib/
```

### 3. Installation

To install the server, client, and headers:
```bash
cmake --install .
```

This will install:
- bin/matching_engine
- bin/matching_engine_client
- include/matching_engine/*.h

You can adjust the install prefix:
```bash
cmake --install . --prefix /usr/local
```

## Running tests

GoogleTest is fetched automatically during the build.

Run tests with:
```bash
ctest
```
or directly:
```bash
./bin/matching_engine_test
```

## Cleaning the Build

- Remove build artifacts only:
```bash
cmake --build . --target clean
```

- Full clean (recommended):
```bash
rm -rf build/
```

## Project Layout

```text
include/           # Public headers
src/               # Library source files
server_main.cpp    # Entry point for server executable
client_main.cpp    # Entry point for client executable
test/              # Unit tests
CMakeLists.txt     # Build configuration
```