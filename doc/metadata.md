# Metadata

Not just any function defined in LLVM IR can be treated as a node. It must have the proper metadata to:
* Differentiate the outputs from the inputs
* Get descriptions of the node, inputs, and outputs

The metadata format is as follows:
