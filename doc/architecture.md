# Chigraph Architecture
Chigraph uses LLVM extensively in it's implementation.

There are 1 to 1 mappings to many LLVM classes to their representations in Chigraph:

## Execution inputs in nodes as `LLVM::Function`

All execution inputs are mapped to `LLVM::Function`s, meaning that a node with 4 execution inputs would have to define 4 separate  `LLVM::Function`s.

### Inputs and outputs

Because nodes in Chigraph can have multiple inputs and outputs, they are not mapped to return values. For example, a node with two inputs and two outputs' signature would look like this to LLVM:

```C
int func(T1* input1, T2* input2, T3* output1, T4* output2)
```
the `int` that is returned is the ID of the execution output.
