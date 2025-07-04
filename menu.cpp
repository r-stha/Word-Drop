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
    outtextxy(250, 280, (char*)"3. Instructions");
    outtextxy(250, 320, (char*)"4. Exit");
}

void showInstructions() {
    cleardevice();
    setbkcolor(BLACK);
    setcolor(WHITE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    outtextxy(250, 130, (char*)" Instructions");
    
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(50, 160, (char*)"1. Type the words that fall from the top of the screen.");
    outtextxy(50, 190, (char*)"2. Press 'Space' to pause the game.");
    outtextxy(50, 220, (char*)"3. Press 'Enter' to submit your typed word.");
    outtextxy(50, 250, (char*)"4. Avoid letting words reach the bottom of the screen.");
    outtextxy(50, 280, (char*)"5. Your score and health increases with each correct word typed.");
    outtextxy(50, 310, (char*)"6. Try to achieve a high score!");
    outtextxy(50, 340, (char*)"     Press any key to return to the menu.");    
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
                        state = INSTRUCTIONS;
                        break;
                    } else if (ch == '4') {
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
        } else if (state == INSTRUCTIONS) {
            cleardevice();
            showInstructions();
            while (!kbhit()) delay(50);
            getch();  // clear key
            state = MENU;
        }
    }

    closegraph();
}
