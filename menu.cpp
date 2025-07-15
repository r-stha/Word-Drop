#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <string>
#include "menu.h"
#include "game.h"

void splashScreen() {
    cleardevice();
    setbkcolor(BLACK);
    setcolor(LIGHTCYAN);
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);  // big text
     char* title = (char*)"Typing Speed Challenge";
    int x = 100, y = 200;
    std::string typed = "";
    PlaySound(TEXT("resources/keyboard_typing.wav"), NULL, SND_FILENAME | SND_ASYNC);
    // Typing animation
    for (int i = 0; title[i] != '\0'; i++) {
        typed += title[i];
        cleardevice();
        outtextxy(x, y, (char*)typed.c_str());
        delay(120);
    }
    delay(500);  // small pause before loading bar

    // Loading bar
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    setcolor(WHITE);
    outtextxy(x + 30, y + 100, (char*)"Loading...");

    int barX = x + 30, barY = y + 130;
    for (int i = 0; i <= 300; i += 10) {
        setfillstyle(SOLID_FILL, GREEN);
        bar(barX, barY, barX + i, barY + 20);
        delay(50);
    }

    delay(400);
    cleardevice();

    // Final instruction
    // settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    // setcolor(LIGHTGREEN);
    // outtextxy(800 / 2 - 150, 600 / 2, (char*)"Press any key to start...");

    // while (!kbhit()) delay(10);  // Wait for key press
    // getch(); // Consume the key
}

void drawMenu() {
    setcolor(GREEN);
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    outtextxy(220, 150, (char*)"TYPING SPEED GAME");

    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(250, 200, (char*)"1. Start Game");
    outtextxy(250, 240, (char*)"2. High Scores");
    outtextxy(250, 280, (char*)"3. Instructions");
    outtextxy(250, 320, (char*)"4. Exit");
}

void showInstructions() {
    
    setcolor(WHITE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    outtextxy(250, 130, (char*)" Instructions");
    
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(50, 160, (char*)"1. Type words that fall from the top of the screen.");
    outtextxy(50, 190, (char*)"2. Press 'Space' to pause the game.");
    outtextxy(50, 220, (char*)"4. Avoid letting words reach the bottom of the screen.");
    outtextxy(50, 250, (char*)"5. Your score increases with each correct word typed.");
    outtextxy(50, 280, (char*)"6. Your health increases with correct word typed.");
    outtextxy(50, 310, (char*)"7. Try to achieve a high score!");
    outtextxy(50, 340, (char*)"     Press any key to return to the menu.");    
}

void showMenu() {
    
    loadHighScores();  // from game.cpp
    GameState state = MENU;
    initwindow(800, 600, (char*)"Typing Speed Game");

    void* bgBuffer = malloc(imagesize(0, 0, 800, 600));
    readimagefile("resources/menu_background.bmp", 0, 0, 800, 600);
    getimage(0, 0, 800, 600, bgBuffer);
    
    HWND hwnd = GetForegroundWindow();
    HICON hIcon = (HICON)LoadImage(NULL, "resources/icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    if (hIcon)
    {
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }

    splashScreen();  // Show the splash screen before the menu
    PlaySound(TEXT("resources/menu_music.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);  // Play background music
    while (state != EXIT) {
        if (state == MENU) {
            putimage(0, 0, bgBuffer, COPY_PUT);
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
                    }else if (ch == 27) {  // ESC key
                        state = EXIT;
                        break;
                    }                    
                }
            }
        }

        if (state == PLAYING) {
            cleardevice();
            free(bgBuffer);
            stopSound();  // Stop the menu music
            runGame();
            state = MENU;
        } else if (state == HIGHSCORES) {
            // cleardevice();
            putimage(0, 0, bgBuffer, COPY_PUT);
            int y = showHighScores();  // can be in game.cpp or separate
            outtextxy(800 / 2 - 150, y + 40, (char*)"Press any key to return to menu");
            while (!kbhit()) delay(50);
            getch();  // clear key
            state = MENU;
        } else if (state == INSTRUCTIONS) {
            // cleardevice();
            putimage(0, 0, bgBuffer, COPY_PUT);
            showInstructions();
            while (!kbhit()) delay(50);
            getch();  // clear key
            state = MENU;
        }
    }
    free(bgBuffer);
    closegraph();
}
