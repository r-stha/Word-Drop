#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") // Link the Windows Multimedia library
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
#include "game.h"

using namespace std;

struct Word
{
    string text;
    int x, y;
    bool active;
};

struct HighScore
{
    string name;
    int score;
};

vector<HighScore> highScores;

vector<string> dictionary;

vector<Word> words;

string typed = "";

int score = 0, health = 5, fallSpeed = 1, frameCount = 0, lastMilestone = 0;
int screenWidth = 800, screenHeight = 600;
bool showCursor = true;
bool paused = false;
bool gameOver = false;
bool restartRequested = false;

void loadWordsFromFile(const string& filename) {
    ifstream fin(filename);
    string word;
    dictionary.clear();
    while (getline(fin, word)) {
        if (!word.empty()) {
            dictionary.push_back(word);
        }
    }
    fin.close();
}

void loadHighScores()
{
    ifstream fin("file/highscore.txt");
    highScores.clear();
    string name;
    int score;
    while (fin >> name >> score)
    {
        highScores.push_back({name, score});
    }
    fin.close();
}

void saveHighScores()
{
    ofstream fout("file/highscore.txt");
    for (auto &entry : highScores)
    {
        fout << entry.name << " " << entry.score << endl;
    }
    fout.close();
}

void insertHighScore(const string &name, int score)
{
    highScores.push_back({name, score});
    sort(highScores.begin(), highScores.end(), [](HighScore a, HighScore b)
         { return a.score > b.score; });
    if (highScores.size() > 5)
    {
        highScores.resize(5); // Keep only top 5
    }
    saveHighScores();
}

void playSound(const char *filename)
{
    PlaySoundA(filename, NULL, SND_FILENAME | SND_ASYNC);
}

string getRandomWord()
{
    return dictionary[rand() % dictionary.size()];
}

void drawHUD()
{
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2); // larger font

    char buffer[100];

    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(5, 5, 250, 90); // Score area
    setcolor(WHITE);
    rectangle(5, 5, 250, 90);

    sprintf(buffer, "Score: %d", score);
    outtextxy(10, 10, buffer);

    sprintf(buffer, "Health: %d", health);
    outtextxy(10, 40, buffer);

    sprintf(buffer, "Typed: ");
    outtextxy(10, 70, buffer);

    string displayText = typed;
    if (showCursor)
    {
        displayText += "|";
    }

    char typedBuffer[100];
    strcpy(typedBuffer, displayText.c_str()); // convert string to char*
    outtextxy(100, 70, typedBuffer);          // display typed text
}

void drawWords()
{
    // setcolor(YELLOW);
    int idx = 0;
    for (auto &word : words)
    {
        if (word.active)
        {
            word.y += fallSpeed;
            setcolor(DARKGRAY);
            outtextxy(word.x+2, word.y+2, const_cast<char *>(word.text.c_str()));

            int color = 1 + rand() % 14; // Random color from 1 to 14
            setcolor(color);
            outtextxy(word.x, word.y, const_cast<char *>(word.text.c_str()));

            if (word.y >= screenHeight - 20)
            {
                words.erase(words.begin() + idx);
                word.active = false;
                playSound("resources/wrong.wav"); // play sound when word falls off
                health--;
            }
            idx++;
        }
    }
}

void removeMatchedWord()
{
    int idx = 0;
    for (auto &word : words)
    {
        if (word.active && word.text == typed)
        {
            playSound("resources/correct.wav"); // after scoring
            word.active = false;
            score++;
            words.erase(words.begin() + idx); // remove matched word
            if (health < 5)                   // Prevent health from exceeding max
                health++;                     // Increase health on correct word
            typed = "";
            return;
        }
        idx++;
    }

    playSound("resources/wrong.wav");
    health--;   // Decrease health if no match
    typed = ""; // Clear typed text on mismatch
}

Word createWord()
{
    Word newWord;
    newWord.text = getRandomWord();
    newWord.x = 100 + rand() % (screenWidth - 170);
    newWord.y = 0;
    newWord.active = true;
    return newWord;
}

void spawnNewWords()
{
    if (frameCount % 50 == 0)
    { // spawn a new word every 50 frames
        if (words.size() >= 10)
            return; // limit to 10 active
        Word newWord = createWord();
        words.push_back(newWord);
    }
}

void updateSpeed()
{
    if (score % 5 == 0 && score != 0 && score != lastMilestone)
    {
        fallSpeed++;
        lastMilestone = score;
    }
}

void resetGame()
{
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

void showPauseScreen()
{
    setcolor(LIGHTBLUE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    outtextxy(screenWidth / 2 - 120, screenHeight / 2, (char *)"PAUSED");
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(screenWidth / 2 - 140, screenHeight / 2 + 40, (char *)"Press 'Space' to Resume");
}

int showHighScores(){
    setcolor(RED);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(screenWidth / 2 - 150, 160, (char *)"High Scores:");

    int y = 190;
    for (auto &entry : highScores)
    {
        char line[100];
        sprintf(line, "%-10s  %3d", entry.name.c_str(), entry.score);
        outtextxy(screenWidth / 2 - 150, y, line);
        y += 30;
    }
    return y;
}

void showGameOverScreen()
{
    setcolor(RED);
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    outtextxy(screenWidth / 2 - 100, 100, (char *)"GAME OVER");

    int y = showHighScores();

    char buffer[100];
    sprintf(buffer, "Your Score: %d", score);
    outtextxy(screenWidth / 2 - 150, y + 20, buffer);

    outtextxy(screenWidth / 2 - 150, y + 60, (char *)"Press 'R' to Restart");
}

void handleTyping()
{
    if (kbhit()) {
        char ch = getch();
        if (ch == ' ') paused = !paused;
        else if ((ch == 'r' || ch == 'R') && gameOver) restartRequested = true;
        else if (ch == 27) exit(0);
        if (!paused && !gameOver) {
            if (ch == '\b' && !typed.empty()) typed.pop_back();
            else if (ch == '\r') removeMatchedWord();
            else if (isalpha(ch)) typed += ch;
        }
    }
}

void playOpeningTransition() {
    for (int i = 0; i < screenWidth; i += 20) {
        setfillstyle(SOLID_FILL, LIGHTCYAN);
        bar(i, 0, i + 20, screenHeight);
        delay(10);
    }
}

void runGame()
{
    loadWordsFromFile("file/words.txt"); 
    srand(time(0));
    initwindow(screenWidth, screenHeight, (char *)"Typing Speed Game");
    setbkcolor(BLACK);                        // black background
    setcolor(WHITE);                          // white text
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2); // readable size

    do
    {
        resetGame();
        playOpeningTransition();// play opening transition
        while (!restartRequested)
        {
            handleTyping();

            if (!paused && !gameOver)
            {
                cleardevice();
                // readimagefile("resources/background.bmp", 0, 0, screenWidth, screenHeight);
                drawHUD();
                drawWords();
                spawnNewWords();
                updateSpeed();
                frameCount++;
                if (frameCount % 10 == 0) { // toggle every ~0.5 seconds (10 × 50ms delay)
                    showCursor = !showCursor;
                }
            }
            else
            {
                cleardevice();
                drawHUD();

                if (paused)
                {
                    showPauseScreen();
                }
                else if (gameOver)
                {
                    showGameOverScreen();
                }
            }

            if (health <= 0 && !gameOver)
            {
                playSound("resources/game_over.wav");
                playOpeningTransition();
                gameOver = true;

                loadHighScores();

                if (highScores.size() < 5 || score > highScores.back().score)
                {
                    char name[20];
                    cleardevice();
                    setcolor(WHITE);
                    outtextxy(screenWidth / 2 - 150, screenHeight / 2 - 20, (char *)"New High Score! Enter Name: ");
                    int i = 0;
                    while (true)
                    {
                        if (kbhit())
                        {
                            char ch = getch();
                            if (ch == '\r')
                                break;
                            if (ch == '\b' && i > 0)
                            {
                                i--;
                                name[i] = '\0';
                            }
                            else if (isalpha(ch) && i < 19)
                            {
                                name[i++] = ch;
                                name[i] = '\0';
                            }
                            cleardevice();
                            outtextxy(screenWidth / 2 - 150, screenHeight / 2 - 20, (char *)"New High Score! Enter Name: ");
                            outtextxy(screenWidth / 2 - 150, screenHeight / 2 + 20, name);
                        }
                    }

                    insertHighScore(name, score);
                }
            }

            delay(50);
        }

    } while (restartRequested);

    closegraph();
}