#include <string>
#include <clocale>
#include "draw.h"
#include "engine.h"


void ldemo(info& in) { //test values
    in.bname = "Stockfish";
    in.wname = "You";
    
    in.belo = 1500;
    in.welo = -1;
    
    in.evalCP = 35;
}

std::string sqn(int r, int c){ //returns the square selected
    return std::string(1, 'a' + c) + std::string(1, '0' + (8 - r));
}

int main() {
    setlocale(LC_ALL, "C.UTF-8");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(50);

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

    engine eng;
    bool engOn = false;
    bool think = false;
    bool sWhite = true;

    auto newGame = [&]() { 
        b = board();
        in.mlist.clear();
        in.ulist.clear();
        in.evalCP = 0;
        in.evalMate = false;
        in.check = false;
        in.mate = false;
        sr = -1;
        sc = -1;
        think = false;
        if (engOn == true){
            eng.send("stop");
            eng.send("ucinewgame");
        }
    };

    while (true){

        if (engOn == true){
            std::string line;
            while (eng.rLine(line) == true){
                if (think == false){
                    continue;
                }

                if (line.rfind("info ", 0) == 0){
                    size_t p = line.find(" score cp ");
                    if (p != std::string::npos){
                        if (sWhite == true){
                            in.evalCP = atoi(line.c_str() + p + 10);
                        }
                        else {
                            in.evalCP = -atoi(line.c_str() + p + 10);
                        }
                        in.evalMate = false;
                    }
                    else {
                        p = line.find(" score mate ");
                        if (p != std::string::npos){
                            in.evalMate = true;
                            if (sWhite == true){
                                in.mIn = atoi(line.c_str() + p + 12);
                            }
                            else {
                                in.mIn = -atoi(line.c_str() + p + 12);
                            }
                        }
                    }
                }
                else if (line.rfind("bestmove", 0) == 0){
                    think = false; 
                    std::string rest = line.size() > 9 ? line.substr(9) : "";
                    std::string mv = rest.substr(0, rest.find(' '));

                    if (mv.size() < 4){
                        in.mate = true;
                    }
                    else {
                        in.mlist.push_back(b.toSAN(mv));
                        b.applym(mv);
                        in.ulist.push_back(mv);
                    }
                }
            }
        }
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
            char pathbuffer[128] = "";
            echo();
            curs_set(1);
            timeout(-1);
            mvprintw(stusmargin + 3, 0, "Engine path: ");
            clrtoeol();
            getnstr(pathbuffer, 127);
            noecho();
            curs_set(0);
            timeout(50);

            if (pathbuffer[0] != '\0'){
                engPath = pathbuffer;
                eng.stop();
                engOn = false;
                think = false;
                if (eng.start(engPath) == true && eng.hshake()){
                    engOn = true;
                    in.bname = eng.name.empty() ? engPath : eng.name;

                    if (eng.helo == true){
                        int elo = std::max(eng.elomin, std::min(eng.elomax, 1500));
                        eng.send("setoption name UCI_LimitStrength value true");
                        eng.send("setoption name UCI_Elo value " + std::to_string(elo));
                        in.belo = elo;
                    }
                    else {
                        in.belo = -1;
                    }
                    newGame();
                }
                else {
                    eng.stop();
                    in.bname = "(engine failed)";
                    in.belo = -1;
                }
            }
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
            bool cMove = !(engOn == true && (think == true || wtm == false)) && in.mate == false;

            if (cMove == true){
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
                    in.mlist.push_back(b.toSAN(m));
                    b.applym(m);
                    in.ulist.push_back(m);
                    sr = -1;
                    sc = -1;

                    if (engOn == true){
                        std::string pos = "position startpos moves";
                        for (auto& u : in.ulist){
                            pos += " " + u;
                        }
                        eng.send(pos);
                        eng.send("go movetime 1567");
                        sWhite = in.ulist.size() % 2 == 0;
                        think = true;
                    }
                }
            }
            
        }
    }
    endwin();
}