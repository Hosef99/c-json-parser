# c-json-parser
## Dependencies
- [StringBuilder](https://github.com/Hosef99/string-builder)

## Clone with submodules:
```bash
git clone --recurse-submodules https://github.com/Hosef99/c-json-parser.git
```
Or alternatively, 
1. Clone the repository like usual
```bash
git clone https://github.com/Hosef99/c-json-parser.git
```
2. Initialize the git submodule and update it
```bash
git submodule init
git submodule update --merge --recursive
```

## Compiling
Running `make` will automatically compile everything. The main executable `json-parser` file, and also the `libjsonparser.a` library file.

To compile and run, just run `make run`.

To only compile the library, just run `make lib`.
