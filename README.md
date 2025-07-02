# 🎮 Word Drop

**Word Drop** is a fast-paced reflex and typing game built using `graphics.h` in C++. Words fall from the top of the screen, and your goal is to type them before they hit the bottom. If you miss, you lose health — but if you succeed, you score points and recover health. The longer you survive, the faster the words fall!

---

## 🧠 Features

- ⌨️ Type words to eliminate them
- 💥 Words fall from the top like meteors
- ❤️ Gain or lose health based on performance
- ⚡ Word fall speed increases with score
- 🔊 Sound effects for correct/wrong typing and game over
- 🎯 Real-time scoring and high score tracking
- 🛑 Pause (`Space`) and Restart (`R`) functionality

---

## 🎮 Controls

| Key                  | Action                          |
|----------------------|---------------------------------|
| Type letters         | Build the current word          |
| `Enter`              | Submit the typed word           |
| `Backspace`          | Delete last typed character     |
| `Space`              | Pause / Resume the game         |
| `R`                  | Restart the game after Game Over|

---

## 🛠 Requirements

- C++ compiler (e.g., g++, MinGW)
- `graphics.h` setup (WinBGIm or equivalent)
- Windows OS (for `PlaySound()` and `winmm.lib`)
- `.wav` sound files in a `resources/` folder:
  - `correct.wav`
  - `wrong.wav`
  - `gameover.wav`

---

## 🚀 How to Run

### 🔧 Using Makefile (recommended)

1. Clone this repo
2. Place your `.wav` files in `resources/`
3. Compile with:
   ```bash
   make run

## License
This project is open-source and free to use for educational or personal projects.
