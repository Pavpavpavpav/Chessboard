#include <string>
#include <clocale>
#include "draw.h"


void ldemo(info& in) { //test values
    in.bname = "Stockfish";
    in.wname = "You";
    
    in.belo = 1500;
    in.welo = -1;
    
    in.evalCP = 35;
}

std::string sqn(int r, int c){
    return std::string(1, 'a' + c) + std::string(1, '0' + (8 - r));
}

int main() {
    setlocale(LC_ALL, "C.UTF-8");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

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

    int cr = 6, cc = 4;
    int sr = -1, sc = -1;

    auto newGame = [&]() { 
        b = board();
        in.mlist.clear();
        in.evalCP = 0;
        in.evalMate = false;
        in.check = false;
        in.mate = false;
        sr = -1;
        sc = -1;
    };

    while (true){
        erase();
        drawb(b, cr, cc, sr, sc);
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
        if (ch == 'e' || ch == 'E'){
            //engine stuff here
        }

        if ((ch == KEY_UP || ch == 'i') && cr > 0){
            cr--;
        }
        if ((ch == KEY_DOWN || ch == 'k') && cr < 7){
            cr++;
        }
        if ((ch == KEY_LEFT || ch == 'j') && cc > 0){
            cc--;
        }
        if ((ch == KEY_RIGHT || ch == 'l') && cc < 7){
            cc++;
        }

        if (ch == '\n' || ch == ' ' || ch == KEY_ENTER){
            bool wtm = in.mlist.size() % 2 == 0;
            char piec =  b.sq[cr][cc];
            bool mine = piec != ' ' && (wtm ? isupper(piec) : islower(piec));

            if (sr == cr && sc == cc){
                sr = -1;
                sc = -1;
            }
            else if (mine == true){
                sr = cr;
                sc = cc;
            }
            else if (sr >= 0){
                std::string m = sqn(sr, sc) + sqn(cr, cc);
                
                if ((b.sq[sr][sc] == 'P' && cr == 0) || (b.sq[sr][sc] == 'p' && cr == 7)){
                    m += 'q';
                }
                b.applym(m);
                in.mlist.push_back(m);
                sr = -1;
                sc = -1;
            }
        }
    }
    endwin();
}