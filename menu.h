#ifndef MENU_H
#define MENU_H
#pragma once

enum GameState { MENU, PLAYING, HIGHSCORES, INSTRUCTIONS, EXIT };

void drawMenu();  // function to draw the menu
void showMenu();  // function to display the menu and call game if needed
void showInstructions(); // function to display game instructions
int showHighScores();  // forward declaration
void loadHighScores();  // from game.cpp

#endif