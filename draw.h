#pragma once // draw code here
#include <ncurses.h>
#include <algorithm>
#include <string>
#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include "board.h"

// variables and stuff

const int margint = 3;
const int marginl = 4;
const int ebarmargin = marginl + 8 * 3 + 1;
const int infpanmargin = ebarmargin + 4;
const int stusmargin = margint + 9;


inline void color() {
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE); //black on white
    init_pair(2, COLOR_RED, COLOR_WHITE); //white on white
    init_pair(3, COLOR_BLACK, COLOR_CYAN); // black on black
    init_pair(4, COLOR_RED, COLOR_CYAN); // white on black
    init_pair(5, COLOR_WHITE, COLOR_WHITE); //eval bar white
    init_pair(6, COLOR_BLACK, COLOR_BLACK); //eval bar black
}

inline void drawb(const board& b) { //drawing the board
    mvprintw(0, 0, "CHESS - pav");

    for (int c = 0; c < 8; c++){
        mvaddch(margint - 1, marginl + c * 3 + 1, 'a' + c);
    }

    for (int r = 0; r < 8; r++){
        mvprintw(margint + r, marginl - 3, "%d", 8 - r);
        for (int c = 0; c < 8; c++){
            bool bg = (r + c) % 2 == 0;
            bool white = isupper(b.sq[r][c]);

            int colr;
            if (bg == true){
                if (white == true){
                    colr = 2;
                }
                else {
                    colr = 1;
                }
            }
            else {
                if (white == true){
                    colr = 4;
                }
                else {
                    colr = 3;
                }
            }

            attron(COLOR_PAIR(colr));
            if (white == true){
                attron(A_BOLD);
            }

            const char* glyph;
            switch (toupper(b.sq[r][c])){
                case 'K': glyph = "♚"; break;
                case 'Q': glyph = "♛"; break;
                case 'R': glyph = "♜"; break;
                case 'B': glyph = "♝"; break;
                case 'N': glyph = "♞"; break;
                case 'P': glyph = "♟"; break;
                default: glyph = " "; break;
            }

            mvprintw(margint + r, marginl + c * 3, " %s ", glyph);
            if (white == true){
                attroff(A_BOLD);
            }
            attroff(COLOR_PAIR(colr));
        }
    }
}

inline void drawebar(const info& in){ // evaluation bar drawing in centipawns
    double fract;

    if (in.evalMate == true){
        if (in.mIn > 0){
            fract = 1;
        }
        else {
            fract = 0;
        }
    }
    else {
        int cp = std::max(-500, std::min(500, in.evalCP));
        fract = 0.5 + cp / 1000.0;
    }

    int whiter = (int)(fract * 8 + 0.5);

    for (int i = 0; i < 8; i++){
        bool white = i >= 8 - whiter;
        if (white == true){
            attron(COLOR_PAIR(5));
        }
        else {
            attron(COLOR_PAIR(6));
        }

        mvprintw(margint + i, ebarmargin, " ");

        if (white == true){
            attroff(COLOR_PAIR(5));
        }
        else {
            attroff(COLOR_PAIR(6));
        }
    }

    char buffer[16];
    if (in.evalMate == true){
        snprintf(buffer, sizeof buffer, "M%d", std::abs(in.mIn));
    }
    else {
        snprintf(buffer, sizeof buffer, "%+.2f", in.evalCP / 100.0);
    }
    mvprintw(margint + 8, ebarmargin - 1, "%s", buffer);
}

inline void drawipan(const info& in){ // draw the inforation on the right of the board
    mvprintw(margint, infpanmargin, "Black - %s", in.bname.c_str());
    mvprintw(margint + 1, infpanmargin, "Elo - %s", elot(in.belo).c_str());

    mvprintw(margint + 6, infpanmargin, "White - %s", in.wname.c_str());
    mvprintw(margint + 7, infpanmargin, "Elo - %s", elot(in.welo).c_str());

    mvprintw(margint + 2, infpanmargin, "Move list:");

    int siz = in.mlist.size();
    int totalM = (siz + 1) / 2;
    int startl = std::max(0, totalM - 3);
    for (int i = startl; i < totalM; i++){
        std::string wm = in.mlist[2 * i];
        std::string bm = (2 * i + 1 < siz) ? in.mlist[2 * i + 1] : "";
        mvprintw(margint + 3 + (i - startl), infpanmargin, "%2d. %-6s %s", i + 1, wm.c_str(), bm.c_str());
    }
}

inline void drawstus(const info& in){ //who moves checks and other stuff under the board
    bool wtm = in.mlist.size() % 2 == 0;
    mvprintw(stusmargin, 0, "%s to move", wtm == true ? "White" : "Black");
    if (in.mate == true){
        mvprintw(stusmargin + 1, 0, "Checkmate - %s wins!", wtm == true ? "Black" : "White");
    }
    else if (in.check == true) {
        mvprintw(stusmargin + 1, 0, "Check!");
    }
    mvprintw(stusmargin + 2, 0, "N - New game  Q - Quit  E - pick engine");
}
