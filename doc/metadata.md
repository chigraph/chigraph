# Metadata

Not just any function defined in LLVM IR can be treated as a node. It must have the proper metadata to:
* Differentiate the outputs from the inputs
* Get descriptions of the node, inputs, and outputs

In the module, for each node, there should be a metadata entry with name `chig-<func_name>`, with the format:

```LLVM
!0 = !{!"Description for the function\00", !"num-arguments\00", !"argument description 1\00", !"argument-description 2\00"}
!chig-<func_name> = !{!0}
```
