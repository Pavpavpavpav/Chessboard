#pragma once
#include <cctype>
#include <cstdlib>
#include <string>

// makes thet board
struct board {
    char sq[8][8];

    board() {
        const char* pcs = "rnbqkbnr";

        //sets the pieces up (reminder : try change how the thing functions to make for custom pos setup)
        for (int col = 0; col < 8; col++){
            sq[0][col] = pcs[col];
            sq[1][col] = 'p';
            sq[6][col] = 'P';
            sq[7][col] = toupper(pcs[col]);
            for (int r = 2; r < 6; r++){
                sq[r][col] = ' ';
            }
        }
    }
};