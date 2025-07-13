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
int totalKeystrokes = 0;
int correctKeystrokes = 0;
int longestStrick = 0;
int currentStrick = 0;

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

void stopSound() {
    PlaySoundA(NULL, 0, 0);
}

string getRandomWord()
{
    return dictionary[rand() % dictionary.size()];
}

int countMatchingPrefix(const std::string& typed, const std::string& actual) {
    int len = std::min(typed.size(), actual.size());
    for (int i = 0; i < len; ++i) {
        if (tolower(typed[i]) != tolower(actual[i])) {
            return i; // mismatch happened here
        }
    }
    return len; // all typed chars match
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
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    for (int i = words.size() - 1; i >= 0; --i)
    {
        if (!words[i].active) continue;
        words[i].y += fallSpeed;

        string full = words[i].text;
        int matchLen = countMatchingPrefix(typed, full);

        int x = words[i].x;
        int y = words[i].y;

        // Shadow
        setcolor(DARKGRAY);
        outtextxy(x + 2, y + 2, const_cast<char *>(full.c_str()));

        // Draw matching prefix in highlight (YELLOW)
        if (matchLen > 0)
        {
            string matched = full.substr(0, matchLen);
            setcolor(YELLOW);
            outtextxy(x, y, const_cast<char *>(matched.c_str()));

            // Measure the width of matched text
            int prefixWidth = textwidth(const_cast<char *>(matched.c_str()));

            // Draw the rest in the word's original color
            string rest = full.substr(matchLen);
            setcolor(words[i].color);
            outtextxy(x + prefixWidth, y, const_cast<char *>(rest.c_str()));
        }
        else
        {
            // No match at all, draw full word in original color
            setcolor(words[i].color);
            outtextxy(x, y, const_cast<char *>(full.c_str()));
        }

        // Missed word
        if (words[i].y >= screenHeight - 20)
        {
            playSound("resources/wrong.wav");
            health--;
            words.erase(words.begin() + i);
        }
    }
}


//no need of this function
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

// Returns true if `typed` is a prefix of `w`
bool isPrefix(const string& typed, const string& w)
{
    return w.size() >= typed.size() &&
           equal(typed.begin(), typed.end(), w.begin());
}

// Returns the index of a word that exactly matches `typed`, –1 if none
int findExactMatch(const string& typed)
{
    for (int i = 0; i < (int)words.size(); ++i)
        if (words[i].active && words[i].text == typed)
            return i;
    return -1;
}

void acceptWord(int index)
{
    playSound("resources/correct.wav");
    score++;
    currentStrick++;
    if (health < 5) health++;          // reward a life
    words.erase(words.begin() + index);
    typed.clear();
}

Word createWord()
{
    Word newWord;
    newWord.text = getRandomWord();
    newWord.x = 100 + rand() % (screenWidth - 190);
    int color = 2 + rand() % 14; // Random color from 1 to 14
    newWord.color = color;
    newWord.y = 0;
    newWord.active = true;
    return newWord;
}

void spawnNewWords()
{
    if (frameCount % 50 == 0 || words.empty())
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
    correctKeystrokes = 0;
    totalKeystrokes = 0;
}

void showPauseScreen()
{
    // std::cout << "currentStrick: " << currentStrick << std::endl;
    // std::cout << "longestStrick: " << longestStrick << std::endl;
    setcolor(LIGHTBLUE);
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    outtextxy(screenWidth / 2 - 80, screenHeight / 2, (char *)"PAUSED");
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(screenWidth / 2 - 140, screenHeight / 2 + 40, (char *)"Press 'Space' to Resume");
}

int showHighScores()
{
    setcolor(YELLOW);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    outtextxy(screenWidth / 2 - 150, 160, (char *)"High Scores:");

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
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
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    outtextxy(screenWidth / 2 - 150, 100, (char *)"!!!GAME OVER!!!");

    int y = showHighScores();

    char buffer[100];
    sprintf(buffer, "Your Score: %d", score);
    outtextxy(screenWidth / 2 - 150, y + 20, buffer);

    float accuracy = 0;
    if(totalKeystrokes > 0)
        accuracy = (correctKeystrokes * 100.0f) / totalKeystrokes;
    
    if(accuracy > 90)
        setcolor(GREEN);
    else if(accuracy > 70)
        setcolor(YELLOW);
    else
        setcolor(RED);

    sprintf(buffer, "Accuracy: %.2f%%", accuracy);
    outtextxy(screenWidth / 2 - 150, y + 60, buffer);

    setcolor(YELLOW);
    sprintf(buffer, "Longest Strick: %d words%", longestStrick);
    outtextxy(screenWidth / 2 - 150, y + 100, buffer);

    outtextxy(screenWidth / 2 - 150, y + 140, (char *)"Press 'R' to Restart");
}

void handleTyping()
{
    while (kbhit())           // read *all* queued keys
    {
        char ch = getch();
        // global hot‑keys first
        if (ch == ' ') { paused = !paused; continue; }
        if (ch == 27)  { exit(0); }          // ESC
        if ((ch == 'r' || ch == 'R') && gameOver) {
            restartRequested = true; continue;
        }

        if (paused || gameOver) continue;    // ignore other keys

        /* --- text input --- */
        if (ch == '\b') {                    // back‑space
            if (!typed.empty()) typed.pop_back();
        }
        else if (isalpha(ch)) {            
            totalKeystrokes++;    // count every keystroke
            typed += ch;
        }

        /* --- real‑time checking --- */

        // 3‑a) exact word finished?
        int idx = findExactMatch(typed);
        if (idx != -1) {                     // typed matches a word
            correctKeystrokes += typed.length(); // count correct keystrokes
            acceptWord(idx);
            continue;
        }

        // 3‑b) still a prefix of at least one word?
        bool stillValid = false;
        for (auto& w : words)
            if (w.active && isPrefix(typed, w.text)) {
                stillValid = true;
                break;
            }

        if (!stillValid && !typed.empty()) { // typed can never match
            for (auto& w : words) {
                if (w.active) {
                    int matchLen = countMatchingPrefix(typed, w.text);
                    correctKeystrokes += matchLen;
                    break; // only apply to first matching word
                }
            }

            playSound("resources/wrong.wav");
            longestStrick = currentStrick > longestStrick ? currentStrick : longestStrick;
            currentStrick = 0; // reset current streak
            health--;
            typed.clear();
        }
    }
}

void playOpeningTransition()
{
    // int currentPage = 0;

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
    void* bgBuffer = malloc(imagesize(0, 0, screenWidth, screenHeight));
    readimagefile("resources/background.bmp", 0, 0, screenWidth, screenHeight);
    getimage(0, 0, screenWidth, screenHeight, bgBuffer);

    srand(time(0));
    setbkcolor(BLACK);                        // black background
    setcolor(WHITE);                          // white text
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2); // readable size

    do
    {
        resetGame();
        playOpeningTransition(); // play opening transition
        while (!restartRequested)
        {
            setactivepage(currentPage);     // Draw to this page
            setvisualpage(1 - currentPage); // Display the other page
            putimage(0, 0, bgBuffer, COPY_PUT);

            handleTyping();

            if (!paused && !gameOver)
            {
                // cleardevice();
                
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
    free(bgBuffer);
    closegraph();
}