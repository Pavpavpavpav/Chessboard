#pragma once
#include <cctype>
#include <cstdlib>
#include <string>
#include <vector>

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

// info of the players
struct info {
    std::string wname = "White's name";
    std::string bname = "Black's name";

    int welo = -1;
    int belo = - 1;

    std::vector<std::string> mlist;
    int evalCP = 0;

    bool evalMate = false;
    bool mIn = 0;

    bool check = 0;
    bool mate = 0;
};


std::string elot(int elo){
    if (elo < 0){
        return "n/a";
    }
    else {
        return std::to_string(elo);
    }
}