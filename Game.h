#pragma once
#include "Animation.h"
#include "Word.h"
#include <vector>
#include <string>
#include <raylib.h>
#include "Difficulty.h"
#include "SaveData.h"

class Game {
public:
    Game(const int screen_width, const int screen_height);
    ~Game();
    void update();
    void draw();

private:
    SaveData highScores;

    Difficulty DIFFICULTY = Difficulty::MEDIUM;
    static constexpr float WORD_RATE_UPDATE_TIME = 10.0f;
    const int TEXT_SIZE = 50;
    const int SCORE_SIZE = 35;
    const int TITLE_SIZE = 70;
    const int INSTRUCTION_SIZE = TEXT_SIZE / 2;
    const int TYPING_TEXT_SIZE = 35;
    const std::string HIGH_SCORE_FILE = "highscore.dat";
    static constexpr int MAX_LIVES = 5;
    float multiplierForNewHighscore = 1.0f;
    float highScoreTimer = 0.0f;
    float highScoreDuration = 2.0f;
    bool newHighScoreFlag = false;
    bool firstPlay = false;
    int minusPressedCounter = 0;
    int plusPressedCounter = 0;

    int findLetterSpacing(int fontSize);
    
    void drawHome();
    void drawPlay();
    void drawPause();
    void drawGameOver();
    void drawFlash();
    void drawVolumeBar();

    void loadAudios();
    void unloadAudios();
    
    void updateHome();
    void updatePlay();
    void updatePause();
    void updateGameOver();

    int getHighScoreForDiffLevel();
    void setHighScoreForDiffLevel(int score);
    void loadHighScore();
    void saveHighScore();

    void resetGame();

    void newHighScore();

    enum class GameState {
        HOME,
        PLAY,
        PAUSE,
        GAMEOVER
    } currentState = GameState::HOME;

    std::vector<Word> activeWords;
    std::string typedString;
    int currentMatches;
    const int SCREEN_WIDTH;
    const int SCREEN_HEIGHT;
    int score = 0;
    int lives = MAX_LIVES;
    bool matchesReduced = false;
    bool redFlash;
    int redFlashCounter = 0;
    int beat = 60;

    float wordSpawnTimer = 0.0f;
    float wordSpawnInterval = 2.5f;
    float wordFallSpeed = 80.0f;
    float difficultyTimer = 0.0f;

    const Color typedStringCOlor = {50, 245, 0, 255};

    Sound wordDestroyedSound;
    Sound mistakeSound;
    Sound lifeLostSound;
    Sound gameOverSound;
    Sound typingSound;
    Sound clickSound;
    Sound newHighScoreSound;
    Music mainMenuMusic;
    Music gameplayMusic;

    float cursorBlinkTimer = 0.0f;

    Texture2D heartTexture;
    Animation heartAnims[MAX_LIVES];

};
