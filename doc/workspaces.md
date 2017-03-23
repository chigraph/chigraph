# Workspaces 

Chigraph code is organized into what's called a "workspace". If you've ever written anything in [golang](https://golang.org/), it's similar (actually inspired by that). 

## Basic structure

This is a small sample of the test workspace (located in [`test/codegen/workspace`](https://github.com/chigraph/chigraph/tree/master/test/codegen/workspace))
```bash
.
├── .chigraphworkspace # empty file denoting the root of the workspace
└── src # holds chigraph modules
    ├── addint # folder containing modules for easier organization
    │   └── main.chimod # a main module, meaning an executable
    ├── intermodule
    │   ├── main.chimod
    │   └── printer.chimod # a library module
    └── intprinter.chimod # another library module
```

Modules are named by the path to their `.chimod` file from the `src` folder. For example, the `printer.chimod` file would have the name of `intermodule/printer`, and the `intpritner.chimod` module would have the name `intprinter`. This is what you would enter as a dependency. 

## Main Modules
Modules who have the name `*/main` (no matter what comes before it, so `addint/main` as well as  just `main` both qualify) are main modules. This means they can be executed if you add a `main` function to them. 
