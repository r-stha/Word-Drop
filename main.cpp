#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")  // Link the Windows Multimedia library
#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <algorithm>

using namespace std;

struct Word {
    string text;
    int x, y;
    bool active;
};

vector<string> dictionary = {
    "apple", "code", "game", "fast", "hello",
    "world", "type", "keyboard", "speed", "skill"
};

vector<Word> words;
string typed = "";

int score = 0, health = 5, fallSpeed = 1, frameCount = 0, lastMilestone = 0;
int screenWidth = 800, screenHeight = 600;
bool showCursor = true;
bool paused = false;
bool gameOver = false;
bool restartRequested = false;

int loadHighScore() {
    ifstream fin("typing_speed_game/highscore.txt");
    int hs = 0;
    if (fin) {
        fin >> hs;
    }
    fin.close();
    return hs;
}

void saveHighScore(int newScore) {
    ofstream fout("typing_speed_game/highscore.txt");
    fout << newScore;
    fout.close();
}

void playSound(const char* filename) {
    PlaySound(TEXT(filename), NULL, SND_FILENAME | SND_ASYNC);
}

string getRandomWord() {
    return dictionary[rand() % dictionary.size()];
}

void drawHUD() {
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);  // larger font

    char buffer[100];

    sprintf(buffer, "Score: %d", score);
    outtextxy(10, 10, buffer);

    sprintf(buffer, "Health: %d", health);
    outtextxy(10, 40, buffer);

    sprintf(buffer, "Typed: ");
    outtextxy(10, 70, buffer);

    string displayText = typed;
    if (showCursor) {
        displayText += "|";
    }

    char typedBuffer[100];
    strcpy(typedBuffer, displayText.c_str());  // convert string to char*
    outtextxy(100, 70, typedBuffer);      // display typed text
}


void drawWords() {
    setcolor(YELLOW);
    int idx = 0;
    for (auto &word : words) {
        if (word.active) {
            word.y += fallSpeed;
            outtextxy(word.x, word.y, const_cast<char*>(word.text.c_str()));

            if (word.y >= screenHeight - 20) {
                words.erase(words.begin() + idx);
                word.active = false;
                playSound("resources/wrong.wav");  // play sound when word falls off
                health--;
            }
            idx++;
        }
    }
}

void removeMatchedWord() {
    int idx = 0;
    for (auto &word : words) {
        if (word.active && word.text == typed) {
            word.active = false;
            playSound("resources/correct.wav");  // after scoring
            score++;
            words.erase(words.begin() + idx); // remove matched word
            if(health < 5)  // Prevent health from exceeding max
                health++;  // Increase health on correct word
            typed = "";
            return;
        }
        idx++;
    }
    
    playSound("resources/wrong.wav");
    health--;  // Decrease health if no match
    typed = "";  // Clear typed text on mismatch
}

Word createWord() {
    Word newWord;
    newWord.text = getRandomWord();
    newWord.x = 100 + rand() % (screenWidth - 170);
    newWord.y = 0;
    newWord.active = true;
    return newWord;
}

void spawnNewWords() {
    if (frameCount % 50 == 0 || words.size() < 5) {  // spawn a new word every 50 frames
        if (words.size() >= 10) return;  // limit to 10 active
        Word newWord = createWord();
        words.push_back(newWord);
    }
}

void updateSpeed() {
    if (score % 5 == 0 && score != 0 && score != lastMilestone) {
        fallSpeed++;
        lastMilestone = score;
    }
}

void resetGame() {
    words.clear();
    typed = "";
    score = 0;
    health = 5;
    fallSpeed = 1;
    frameCount = 0;
    lastMilestone = 0;
    paused = false;
    gameOver = false;
    restartRequested = false;
}

void showPauseScreen() {
    setcolor(LIGHTBLUE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    outtextxy(screenWidth / 2 - 120, screenHeight / 2, (char*)"PAUSED");
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(screenWidth / 2 - 140, screenHeight / 2 + 40, (char*)"Press 'Space' to Resume");
}

void showGameOverScreen(int highScore) {
    setcolor(RED);
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    outtextxy(screenWidth / 2 - 100, screenHeight / 2 - 20, (char*)"GAME OVER");

    char hsBuffer[100];
    sprintf(hsBuffer, "High Score: %d", highScore);
    outtextxy(screenWidth / 2 - 100, screenHeight / 2 + 20, hsBuffer);

    sprintf(hsBuffer, "Current Score: %d", score);
    outtextxy(screenWidth / 2 - 100, screenHeight / 2 + 60, hsBuffer);

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(screenWidth / 2 - 100, screenHeight / 2 + 100, (char*)"Press 'R' to Restart");
}

void handleTyping() {
    if (kbhit()) {
        char ch = getch();

        if (ch == ' ') {
            paused = !paused;
        } else if (ch == 'r' || ch == 'R') {
            if (gameOver) {
                restartRequested = true;
            }
        } else if (ch == 27) // ESC key
            exit(0); 

        if (!paused && !gameOver) {
            if (ch == '\b') {
                if (!typed.empty()) typed.pop_back();
            } else if (ch == '\r') {
                removeMatchedWord();
            } else if (isalpha(ch)) {
                typed += ch;
            }
        }
    }
}


int main() {
    int highScore = loadHighScore();

    srand(time(0));
    initwindow(screenWidth, screenHeight, (char*)"Typing Speed Game");
    setbkcolor(BLACK);     // black background
    setcolor(WHITE);       // white text
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2); // readable size

    do {
        resetGame();

        while (!restartRequested) {
            handleTyping();

            if (!paused && !gameOver) {
                cleardevice();
                drawHUD();
                drawWords();
                spawnNewWords();
                updateSpeed();
                frameCount++;
            } else {
                cleardevice();
                drawHUD();

                if (paused) {
                    showPauseScreen();
                } else if (gameOver) {
                    showGameOverScreen(highScore);
                }
            }

            if (health <= 0 && !gameOver) {
                playSound("resources/game_over.wav");
                gameOver = true;

                if (score > highScore) {
                    saveHighScore(score);
                    highScore = score;
                }
            }

            delay(50);
        }

    } while (restartRequested);

    closegraph();
    return 0;
}
