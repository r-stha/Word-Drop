#ifndef GAME_H
#define GAME_H
#pragma once

void loadHighScores();
void saveHighScores();
void playSound(const char *filename);
void stopSound();
void drawHUD(); // draws the HUD with score, health, and typed text
void drawWords(); // draws the falling words on the screen
void removeMatchedWord(); // removes matched word from the screen
void spawnNewWords(); // spawns new words on the screen
void updateSpeed(); // updates the speed of falling words
void resetGame(); // resets the game state
void showPauseScreen(); // displays the pause screen
int showHighScores();  
void showGameOverScreen(void*); // displays the game over screen
void handleTyping(); // handles user input for typing
void runGame();           // starts the game
void playOpeningTransition(); // displays high scores

#endif
