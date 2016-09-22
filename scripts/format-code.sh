#!/bin/bash


clang-format -style=file -i $(find . -name "*.cpp") $(find . -name "*.hpp")

