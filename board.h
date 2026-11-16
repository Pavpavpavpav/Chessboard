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

    void applym(const std::string& m){ // does the moving stuff (Hi if anyone tweaking this keep inmidn that this takes UCI moves so that means it explodes if it doesnt recieve one !!!)
        int fc = m[0] - 'a';
        int fr = 8 - (m[1] - '0');

        int tc = m[2] - 'a';
        int tr = 8 - (m[3] - '0');

        char piece = sq[fr][fc];

        if (toupper(piece) == 'K' && abs(tc - fc) == 2){
            int rf;
            int rt;

            if (tc > fc){
                rf = 7;
                rt = 5;
            }
            else {
                rf = 0;
                rt = 3;
            }
            sq[fr][rt] = sq[fr][rf];
            sq[fr][rf] = ' ';
        }
        if (toupper(piece) == 'P' && fc != tc && sq[tr][tc] == ' '){
            sq[fr][tc] = ' ';
        }

        sq[tr][tc] = piece;
        sq[fr][fc] = ' ';

        if (m.size() == 5){
            if (isupper(piece)){
                sq[tr][tc] = toupper(m[4]);
            }
            else {
                sq[tr][tc] = tolower(m[4]);
            }
        }
    }

    std::string toSAN(const std::string&m) const {
        int fc = m[0] - 'a';
        int fr = 8 - (m[1] - '0');

        int tc = m[2] - 'a';
        int tr = 8 - (m[3] - '0');

        char piece = toupper(sq[fr][fc]);

        if (piece == 'K' && abs(tc - fc) == 2) {
            if (tc > fc){
                return "O-O";
            }
            else {
                return "O-O-O";
            }
        }

        bool capt = sq[tr][tc] != ' ' || (piece == 'P' && fc != tc);

        std::string san;

        if (piece == 'P'){
            if (capt == true){
                san += m[0];
            }
        }
        else {
            san += piece;
        }
        if(capt == true){
            san += 'x';
        }
        san += m.substr(2, 2);
        if (m.size() == 5){
            san += '=';
            san += toupper(m[4]);
        }
        return san;
    }
};

// info of the players
struct info {
    std::string wname = "You";
    std::string bname = "(no engine)";

    int welo = -1;
    int belo = -1;

    std::vector<std::string> mlist; //san
    std::vector<std::string> ulist; //uci
    int evalCP = 0;

    bool evalMate = false;
    int mIn = 0;

    bool check = 0;
    bool mate = 0;
};


inline std::string elot(int elo){
    if (elo < 0){
        return "n/a";
    }
    else {
        return std::to_string(elo);
    }
}