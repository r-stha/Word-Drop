#ifndef MENU_H
#define MENU_H
#pragma once

enum GameState { MENU, PLAYING, HIGHSCORES, INSTRUCTIONS, EXIT };

void showMenu();  // function to display the menu and call game if needed
void showInstructions();

#endif