# Codegen tests
Code generation needs testing obviously. These tests have their own JSON format, and goes like so:

{
	"testname": "A brief description of the test",
	"expectedret": "Expected success of the compiled program. 0 for success, other error codes otherwise",
	"expectedstdout": "Expected output of the program",
	"module": { the actual chig module here }
}

