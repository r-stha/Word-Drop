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
    int color;
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

int currentPage = 0;

int score = 0, health = 5, fallSpeed = 1, frameCount = 0, lastMilestone = 0;
int screenWidth = 800, screenHeight = 600;

bool showCursor = true;
bool paused = false;
bool gameOver = false;
bool restartRequested = false;

void loadWordsFromFile(const string &filename)
{
    ifstream fin(filename);
    string word;
    dictionary.clear();
    while (getline(fin, word))
    {
        if (!word.empty())
        {
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
    for (int i = words.size() - 1; i >= 0; --i) {
        if (words[i].active) {
            words[i].y += fallSpeed;

            setcolor(DARKGRAY);
            outtextxy(words[i].x + 2, words[i].y + 2, const_cast<char *>(words[i].text.c_str()));
            setcolor(words[i].color);
            outtextxy(words[i].x, words[i].y, const_cast<char *>(words[i].text.c_str()));

            if (words[i].y >= screenHeight - 20) {
                playSound("resources/wrong.wav");
                health--;
                words.erase(words.begin() + i);
            }
        }
    }
}

void removeMatchedWord()
{
    for (int i = words.size() - 1; i >= 0; --i) {
        if (words[i].active && words[i].text == typed) {
            playSound("resources/correct.wav");
            score++;
            if (health < 5) health++;
            words.erase(words.begin() + i);
            typed = "";
            return;
        }
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
    int color = 1 + rand() % 14; // Random color from 1 to 14
    newWord.color = color;
    newWord.y = 0;
    newWord.active = true;
    return newWord;
}

void spawnNewWords()
{
    if (frameCount % 50 == 0 || words.size() <= 2)
    { // spawn a new word every 50 frames
        if (words.size() >= 8)
            return; // limit to 10 active
        Word newWord = createWord();
        words.push_back(newWord);
    }
}

void updateSpeed()
{
    if (score % 5 == 0 && score != 0 && score != lastMilestone)
    {
        if (fallSpeed < 7) // Limit fall speed to a maximum value
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

int showHighScores()
{
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
    if (kbhit())
    {
        char ch = getch();
        if (ch == ' ')
            paused = !paused;
        else if ((ch == 'r' || ch == 'R') && gameOver)
            restartRequested = true;
        else if (ch == 27)
            exit(0);
        if (!paused && !gameOver)
        {
            if (ch == '\b' && !typed.empty())
                typed.pop_back();
            else if (ch == '\r')
                removeMatchedWord();
            else if (isalpha(ch))
                typed += ch;
        }
    }
}

void playOpeningTransition()
{
    int currentPage = 0;

    for (int i = 0; i < screenWidth; i += 20) {
        setactivepage(currentPage);
        setvisualpage(1 - currentPage);

        cleardevice();
        for (int j = 0; j <= i; j += 20) {
            setfillstyle(SOLID_FILL, LIGHTCYAN);
            bar(j, 0, j + 20, screenHeight);
        }

        delay(10);
        currentPage = 1 - currentPage;
    }

    // After transition, show final frame
    setactivepage(0);
    setvisualpage(0);
}

void runGame()
{
    cleardevice();
    loadWordsFromFile("file/words.txt");
    srand(time(0));
    setbkcolor(BLACK);                        // black background
    setcolor(WHITE);                          // white text
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2); // readable size

    do
    {
        resetGame();
        playOpeningTransition(); // play opening transition
        while (!restartRequested)
        {
            setactivepage(currentPage);     // Draw to this page
            setvisualpage(1 - currentPage); // Display the other page
            cleardevice();

            handleTyping();

            if (!paused && !gameOver)
            {
                // cleardevice();
                readimagefile("resources/background.bmp", 0, 0, screenWidth, screenHeight);
                drawHUD();
                drawWords();
                spawnNewWords();
                updateSpeed();
                frameCount++;
                if (frameCount % 10 == 0)
                { // toggle every ~0.5 seconds (10 × 50ms delay)
                    showCursor = !showCursor;
                }
            }
            else
            {
                // cleardevice();
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
                    char name[20] = "";
                    int i = 0;
                    cleardevice();

                    while (true)
                    {
                        setactivepage(currentPage);
                        setvisualpage(1 - currentPage);
                        cleardevice();
                        setcolor(WHITE);
                        outtextxy(screenWidth / 2 - 150, screenHeight / 2 - 20, (char *)"New High Score!");
                        outtextxy(screenWidth / 2 - 150, screenHeight / 2 + 20, (char *)"Enter Name: ");
                        outtextxy(screenWidth / 2, screenHeight / 2 + 20, name);

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
                        }

                        delay(30);
                        currentPage = 1 - currentPage;
                    }

                    insertHighScore(name, score);
                }
            }

            delay(50);
            currentPage = 1 - currentPage; // Switch pages for double buffering
        }

    } while (restartRequested);

    closegraph();
}