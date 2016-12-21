# Chigraph automation scripts
Scripts are fun! (Plus I'm lazy)

Here are some scripts for things I do a lot or it's just hard to remember how to do it.

# `format_code.sh`
Format the code using [clang format](http://clang.llvm.org/docs/ClangFormat.html). Relies on `clang-format` to be in `$PATH`

It uses the `.clang-format` file for style advice.

# `clang_tidy.sh`
Run [clang tidy](http://clang.llvm.org/docs/ClangTidy.html) on the code. Provides nice style, readability and performance diagnostics

# `build_frameworks.sh`
Compiles the components from KDE Frameworks 5 that chigraph gui relies on.

It is to be used like:
```bash
./build_frameworks.sh <Debug|Release> <CMake Generator>
```

# `setup_hooks.sh`
Setups a hook in git to run `format_code.sh` before each commit

# `create_appimage.sh`
Creates a linux appimage! It deletes the build directory then reruns cmake with the arguments supplied on the command line, then outputs the appimage in build/

Usage:
```bash
./create_appimage.sh <Extra CMake arguments>
```

