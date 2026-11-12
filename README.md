# Personal library for a bunch of useful stuff

Just a bunch of useful code for me, put into a single lib,  
so I don't have to reimplement it every single time.  
Defnitely not the best implementations ever made, but they're mine, so idc.

### Contains:
- "safe" wrappers for "unsafe" functions
- testing framework
- dynamic array implementation
- length aware string slices
- hashmap implementation
- simple build system in C

### Planned:
- heap implementation
- memory arena implementation

# Build
#### 1. Create the build config file `.build.h` from `.build.h.example`:
```
cp .build.h.example .build.h
```
Modify it to your liking.

#### 2. Compile the `build` binary:
```
cc -Icompat -Iinclude build.c -o build
```
The binary will be automatically rebuilt after the first compilation.

#### 3. Run `build` with a chosen target:
```
./build [both|so|a|test|clean]
```
The first target (`both`) will be automatically chosen if no argument is given.
