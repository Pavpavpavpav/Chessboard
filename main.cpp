#include <string>
#include <clocale>
#include "draw.h"
#include "engine.h"




std::string sqn(int r, int c){ //returns the square selected
    return std::string(1, 'a' + c) + std::string(1, '0' + (8 - r));
}

int main(int argc, char* argv[]) {
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
    std::string engPath;

    int cr = 6, cc = 4;
    int sr = -1, sc = -1;

    engine eng;
    bool engOn = false;
    bool think = false;
    bool sWhite = true;
    int igbest = 0;

    auto goEng = [&]() { // engine moves
        std::string pos = "position startpos moves";
        for (auto& u : in.ulist){
            pos += " " + u;
        }
        eng.send(pos);
        eng.send("go movetime 1567");
        sWhite = in.ulist.size() % 2 == 0;
        think = true;
    };

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
        cr = in.pWhite ? 6 : 1;
        cc = 4;

        if (engOn == true){
            if (think == true){
                eng.send("stop");
                igbest++;
            }
            eng.send("ucinewgame");
        }
        think = false;

        if (engOn == true && in.pWhite == false){
            goEng();
        }
    };

    auto connect = [&](const std::string& path) -> bool {
        eng.stop();
        engOn = false; 
        in.engine = false; 
        think = false; 
        igbest = 0;

        if (eng.start(path) == true && eng.hshake() == true){
            engOn = true;
            in.engine = true;

            std::string& en = in.pWhite ? in.bname : in.wname;
            int& eelo = in.pWhite ? in.belo : in.welo;

            en = eng.name.empty() ? path : eng.name;
            if (eng.helo == true){
                int elo = std::max(eng.elomin, std::min(eng.elomax, 1500));
                eng.send("setoption name UCI_LimitStrength value true");
                eng.send("setoption name UCI_Elo value " + std::to_string(elo));
                engElo = elo;
            }
            else {
                engElo = -1;
            }
            newGame();
            return true;
        }
        eng.stop();
        return false;
    }

    std::string self = argv[0]; //tried to find def engine at startup (! change when i make new engine)
    size_t slash = self.rfind('/');
    std::string dir = slash == std::string::npos ? "." : self.substr(0, slash);
    std::string bundled = dir + "/engines/stockfish";

    const char* defaults[] = {bundled.c_str(), "/usr/games/stockfish", "stockfish"};
    for (const char* d : defaults){
        if (connect(d) == true){
            engPath = d;
            break;
        }
    }

     while (true){

        if (engOn == true){
            std::string line;
            while (eng.rLine(line) == true){
                if (line.rfind("bestmove", 0) == 0 && igbest > 0){
                    igbest--;
                    continue;
                }
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
        drawb(b, cr, cc, sr, sc, !in.pWhite);
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
        if (ch == 's' || ch == 'S'){
            in.pWhite = !in.pWhite;
            std::swap(in.wname, in.bname);
            std::swap(in.welo, in.belo);
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
                if (connect(engPath) == false){
                    (in.pWhite ? in.bname : in.wname) = "(engine failed)";
                }
            }
        }

        int vert = in.pWhite ? -1 : 1;          
        int horiz = in.pWhite ? 1 : -1;       

        if (ch == KEY_UP || ch == 'i'){
            int n = cr + vert;
            if (n >= 0 && n <= 7){
                cr = n;
            }
        }
        if (ch == KEY_DOWN || ch == 'k'){
            int n = cr - vert;
            if (n >= 0 && n <= 7){
                cr = n;
            }
        }
        if (ch == KEY_RIGHT || ch == 'l'){
            int n = cc + horiz;
            if (n >= 0 && n <= 7){
                cc = n;
            }
        }
        if (ch == KEY_LEFT || ch == 'j'){
            int n = cc - horiz;
            if (n >= 0 && n <= 7){
                cc = n;
            }
        }

        if (ch == '\n' || ch == ' ' || ch == KEY_ENTER){
            bool wtm = in.mlist.size() % 2 == 0;
            bool myTurn = (wtm == in.pWhite);
            bool canMove = in.mate == false && (engOn == false || (think == false && myTurn == true));

            if (canMove == true){
                char piec = b.sq[cr][cc];
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
                        goEng();
                    }
                }
            }
        }
    }

    eng.stop();
    endwin();
}