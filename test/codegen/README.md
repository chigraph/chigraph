# Codegen tests
Code generation needs testing obviously. These tests have their own JSON format, and goes like so:

```json
{
	"testname": "A brief description of the test",
	"expectedret": "Expected success of the compiled program. 0 is usually for success, other error codes otherwise",
	"expectedstdout": "Expected stdout of the program",
	"expectedstderr": "Expected stderr of the program"
}
```

This file is to be placed right next to the `main.chimod` for the test
