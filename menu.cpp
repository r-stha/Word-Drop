#include <graphics.h>
#include <conio.h>
#include "menu.h"
#include "game.h"  // includes game loop

void drawMenu() {
    cleardevice();
    setbkcolor(BLACK);
    setcolor(WHITE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    outtextxy(200, 100, (char*)"TYPING SPEED GAME");

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(250, 200, (char*)"1. Start Game");
    outtextxy(250, 240, (char*)"2. High Scores");
    outtextxy(250, 280, (char*)"3. Exit");
}

int showHighScores();  // forward declaration
void loadHighScores();  // from game.cpp

void showMenu() {
    loadHighScores();  // from game.cpp
    GameState state = MENU;
    initwindow(800, 600, (char*)"Typing Speed Game");

    while (state != EXIT) {
        if (state == MENU) {
            drawMenu();
            while (1) {
                if (kbhit()) {
                    char ch = getch();
                    if (ch == '1') {
                        state = PLAYING;
                        break;
                    } else if (ch == '2') {
                        state = HIGHSCORES;
                        break;
                    } else if (ch == '3') {
                        state = EXIT;
                        break;
                    }
                }
            }
        }

        if (state == PLAYING) {
            cleardevice();
            playOpeningTransition(); // from game.cpp
            runGame();  // from game.cpp
            state = MENU;
        } else if (state == HIGHSCORES) {
            cleardevice();
            int y = showHighScores();  // can be in game.cpp or separate
            outtextxy(800 / 2 - 150, y + 40, (char*)"Press any key to return to menu");
            while (!kbhit()) delay(50);
            getch();  // clear key
            state = MENU;
        }
    }

    closegraph();
}
