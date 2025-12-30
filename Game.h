#pragma once
#include "Animation.h"
#include "Word.h"
#include <vector>
#include <string>
#include <raylib.h>

class Game {
public:
    Game(const int screen_width, const int screen_height);
    ~Game();
    void update();
    void draw();

private:

    const int TEXT_SIZE = 50;
    const int TYPING_TEXT_SIZE = 35;
    const int WORD_FALLING_SPEED = 2.75;
    const int WORD_RATE_UPDATE_TIME = 8;
    const std::string HIGH_SCORE_FILE = "highscore.txt";
    static constexpr int MAX_LIVES = 5;

    void resetTimer();
    void drawHome();
    void drawPlay();
    void drawPause();
    void drawGameOver();
    void loadAudios();
    void unloadAudios();
    
    void updateHome();
    void updatePlay();
    void updatePause();
    void updateGameOver();

    void loadHighScore();
    void saveHighScore();

    void resetGame();

    enum class GameState {
        HOME,
        PLAY,
        PAUSE,
        GAMEOVER
    } currentState = GameState::HOME;

    std::vector<Word> activeWords;
    int timer;
    std::string typedString;
    int currentMatches;
    const int SCREEN_WIDTH;
    const int SCREEN_HEIGHT;
    int score = 0;
    int seconds = 0;
    int frameCounter = 0;
    int wordRate = 90;
    int lives = MAX_LIVES;
    int highScore;
    bool matchesReduced = false;

    Sound wordDestroyedSound;
    Sound mistakeSound;
    Sound lifeLostSound;
    Sound gameOverSound;
    Sound typingSound;
    Music mainMenuMusic;
    Music gameplayMusic;

    float cursorBlinkTimer = 0.0f;

    Texture2D heartTexture;
    Animation heartAnims[MAX_LIVES];

};
