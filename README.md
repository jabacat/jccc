# JCCC (JabaCat C Compiler)
A C compiler and preprocessor written from scratch in C that targets x86-64 assembly

![CMake GitHub Workflow](https://img.shields.io/github/actions/workflow/status/jabacat/jccc/cmake.yml?style=for-the-badge)

## Build Instructions

Make sure you have `cmake` installed on your machine, then run
`./scripts/build.sh`. Then, you can run JCCC with `./build/jccc`.

## Usage

Only lexing is supported at this stage. To lex, run the following:
`jccc --token-dump <filename>`
