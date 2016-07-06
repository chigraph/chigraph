[![Travis](https://img.shields.io/travis/GuapoTaco/chigraph.svg?maxAge=2592000)](https://travis-ci.org/GuapoTaco/chigraph) [![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)](https://opensource.org/licenses/MIT) [![Documentation](https://img.shields.io/badge/documentation-online-brightgreen.svg?style=flat)](https://GuapoTaco.github.io/chigraph)

# chigraph
A systems language that is not in text--it is a flowgraph.

## Motivation
There are many many emerging graph-based *scripting* libraries, but that's the problem: *scirpting*. Chigraph is a systems language, compiled via LLVM, to create native speed binaries.

## Text based format
The major problem I have with most flow graph libraries today is their binary formats. This makes it extremly hard to collaborate using source control, which is a main part of software development. Chigraph uses a totally human-readable JSON format, which allows developers to be smart when fixing merge errors.

## Fast runtime
Because it is optimized and compiled by LLVM, chigraph has speeds similar to those of C++.

## Easy bindings
Each node is just a fancy function call, so bindings for C/C++ libraries should be a breeze.

## Implementation
* Rendering using cairo
* All backends/base nodes using C++
* Compilation using LLVM
