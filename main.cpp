#include <ncurses.h>
#include "board.h"

void color() {
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE); //black on white
    init_pair(2, COLOR_RED, COLOR_WHITE); //white on white
    init_pair(3, COLOR_BLACK, COLOR_CYAN); // black on black
    init_pair(4, COLOR_RED, COLOR_CYAN); // white on black
}

void draw(const board& b) {

    int margint = 3;
    int marginl = 4;

    for (int c = 0; c < 8; c++){
        mvaddch(margint - 1, marginl + c * 2, 'a' + c);
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

            mvprintw(margint + r, margint + c * 2, "%c ", b.sq[r][c] == ' ' ? ' ' : toupper(b.sq[r][c]));
            if (white == true){
                attroff(A_BOLD);
            }
            attroff(COLOR_PAIR(colr));
        }
    }
    for (int c = 0; c < 8; c++){
        mvaddch(8 * 3, 3 + c * 6 + 6 / 2, 'a' + c);
    }
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
    draw(b);
    mvprintw(8 * 3 + 2, 0, "Press any key to exit...");
    refresh();

    getch();
    endwin();
}