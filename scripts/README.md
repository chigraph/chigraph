# Chigraph automation scripts
Scripts are fun! (Plus I'm lazy)

Here are some scripts for things I do a lot or it's just hard to remember how to do it.

# `format_code.sh`
Format the code using [clang format](http://clang.llvm.org/docs/ClangFormat.html). Relies on `clang-format` to be in `$PATH`

It uses the `.clang-format` file for style advice.

# `clang_tidy.sh`
Run [clang tidy](http://clang.llvm.org/docs/ClangTidy.html) on the code. Provides nice style, readability and performance diagnostics

# `generate_kf5_tarballs.sh`
Compiles the components from KDE Frameworks 5 that chigraph gui relies on.
It does it in a [docker](https://www.docker.com/) image, so [docker](https://www.docker.com/) must be installed activated.
It outputs two tarballs in the scripts directory.

# `setup_hooks.sh`
Setups a hook in git to run `format_code.sh` before each commit

# `generate_appimage.sh`
Requies [docker](https://www.docker.com/) to be intalled and activated.
Warning: This will take quite a while, approx 20-30 minutes.

Outputs an appimage in the scripts directory

