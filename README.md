[![Travis](https://img.shields.io/travis/GuapoTaco/chigraph.svg?maxAge=2592000?style=plastic)](https://travis-ci.org/GuapoTaco/chigraph)

# chigraph
A systems language implemented in a flow graph using LLVM

## Motivation
There are many many emerging graph-based *scripting* libraries, but that's the problem: *scirpting*. Chigraph is a systems language, compiled via LLVM, to create native speed binaries.

## Text based format
The major problem I have with most flow graph libraries today is their binary formats. This makes it extremly hard to collaborate using source control, which is a main part of software development. Chigraph uses a totally human-readable JSON format, which allows developers to be smart when fixing merge errors.

## Fast runtime
Because it is optimized and compiled by LLVM, the runtime os chigraphs is extremely fast.

## Easy bindings
Each node is just a fancy function call, so bindings for C/C++ libraries should be a breeze.

## Implementation
* Rendering using cairo
* All backends/base nodes using C++
* Compilation using LLVM
