#include <string>
#include "draw.h"


void ldemo(info& in) { //test values
    in.bname = "Stockfish";
    in.wname = "You";
    
    in.belo = 1500;
    in.welo = -1;
    
    in.mlist = {"e4", "e5", "Nf3", "Nc6", "Bb5", "a6", "Ba4"};
    in.evalCP = 35;
}


int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    if (has_colors() == false){
        endwin();
        printf("This terminal doesn't support colors.\n");
        printf("Press any key to exit... \n");
        getch();
        return 1;
    }
    color();

    board b;
    info in;
    ldemo (in);
    std::string engPath;

    auto newGame = [&]() { 
        b = board();
        in.mlist.clear();
        in.evalCP = 0;
        in.evalMate = false;
        in.check = false;
        in.mate = false;
    };

    while (true){
        erase();
        drawb(b);
        drawebar(in);
        drawipan(in);
        drawstus(in);
        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        if (ch == 'n' || ch == 'N'){
            newGame();
        }
        if (ch == 'e' || 'E'){
            //engine stuff here
        }
    }
    drawb(b);
    mvprintw(8 * 3 + 2, 0, "Press any key to exit...");
    refresh();

    getch();
    endwin();
}