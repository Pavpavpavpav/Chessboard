#!/bin/sh

# builds Stockfish and the program stuff

set -e

mkdir -p engines
if [ ! -f engines/stockfish ]; then
    git clone --depth 1 https://github.com/official-stockfish/Stockfish.git engines/stockfish-src
    (cd engines/stockfish-src/src && make -j build ARCH="${ARCH:-x86-64-avx2}")
    cp engines/stockfish-src/src/stockfish engines/stockfish
fi

g++ -std=c++17 main.cpp -lncurses -o chess
echo "Done. Run ./chess"