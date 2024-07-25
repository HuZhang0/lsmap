# Logic synthesis mapping tool (Lsmap)

Lsmap is based on the [EPFL Logic Synthesis Libraries](https://github.com/lsils/lstools-showcase). We implement efficient choice mapping and optimization tools based on [Mockturtle](https://github.com/lsils/mockturtle).

## Requirements

A modern compiler is required to build the libraries. Compiled successfully with Clang 6.0.1, Clang 12.0.0, GCC 7.3.0, GCC 8.2.0,and GCC 8.4.0.

## How to Compile

```bash
git clone --recursive https://github.com/HuZhang0/lsmap.git
cd lsmap
mkdir build
cd build
cmake ..
make
./build/bin/lsmap
```

You can specify the path of the complier by extending _cmake_ as follows:

```bash
cmake -DCMAKE_CXX_COMPILER=/path/to/c++-compiler ..
```

Please execute

```bash
git submodule update --recursive
```

if the third-party libraries are updated.

Then, run

```bash
git pull origin master
```

for the main repository updates.
