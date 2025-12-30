#include "Game.h"
#include "Word.h"
#include "WordBank.h"
#include <cmath>
#include <fstream>

Game::Game(const int screen_width, const int screen_height)
    : SCREEN_WIDTH(screen_width),
      SCREEN_HEIGHT(screen_height),
      wordSpawnTimer(0.0f),
      difficultyTimer(0.0f),
      wordSpawnInterval(2.0f),
      wordFallSpeed(80.0f) {

    heartTexture = LoadTexture("assets/art/Hearts/heart_animated_1.png");

    for (int i = 1; i < MAX_LIVES; i++) {
        heartAnims[i] = Animation(0, 4, 1, 0.1f, Animation::AnimationType::Once);
    }

    heartAnims[0] = Animation(0, 4, 1, 0.1f, Animation::AnimationType::PingPong);

    loadHighScore();
    loadAudios();

    SetMusicVolume(mainMenuMusic, 1.0f);
    SetMusicVolume(gameplayMusic, 1.0f);
    currentState = GameState::HOME;

    PlayMusicStream(mainMenuMusic);
    resetGame();
}


Game::~Game() {
    UnloadTexture(heartTexture);
    unloadAudios();
}

void Game::loadHighScore() {
    std::ifstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        file >> highScore;
        file.close();
    } else {
        highScore = 0;
    }
}

void Game::saveHighScore() {
    std::ofstream file(HIGH_SCORE_FILE);
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

void Game::update() {
    switch (currentState)
    {
    case GameState::HOME:
        updateHome();
        break;
    
    case GameState::PLAY:
        updatePlay();
        break;
    
    case GameState::PAUSE:
        updatePause();
        break;
    
    case GameState::GAMEOVER:
        updateGameOver();
        break;
    
    default:
        break;
    }
}

void Game::updateHome() {
    UpdateMusicStream(mainMenuMusic);
    if(IsKeyPressed(KEY_ENTER)) {
        currentState = GameState::PLAY;
        StopMusicStream(mainMenuMusic);
        PlayMusicStream(gameplayMusic);
    }
}

void Game::loadAudios() {
    wordDestroyedSound = LoadSound("assets/audio/sound_effects/word_destroyed.wav");
    mistakeSound = LoadSound("assets/audio/sound_effects/mistake.wav");
    lifeLostSound = LoadSound("assets/audio/sound_effects/lose_life.wav");
    gameOverSound = LoadSound("assets/audio/sound_effects/game_over.wav");
    typingSound = LoadSound("assets/audio/sound_effects/typing.wav");
    mainMenuMusic = LoadMusicStream("assets/audio/music/menu_music.wav");
    gameplayMusic = LoadMusicStream("assets/audio/music/blah.mp3");
}

void Game::unloadAudios() {
    UnloadSound(wordDestroyedSound);
    UnloadSound(mistakeSound);
    UnloadSound(lifeLostSound);
    UnloadSound(gameOverSound);
    UnloadMusicStream(mainMenuMusic);
    UnloadMusicStream(gameplayMusic);
}

void Game::updatePlay() {
    UpdateMusicStream(gameplayMusic);

    float dt = GetFrameTime();
    matchesReduced = false;

    if(IsKeyPressed(KEY_SPACE)) {
        currentState = GameState::PAUSE;
        SetMusicVolume(gameplayMusic, 0.2f);
        return;
    }

    wordSpawnTimer += dt;
    if (wordSpawnTimer >= wordSpawnInterval) {
        wordSpawnTimer -= wordSpawnInterval;

        std::string newWord = WordBank::getRandomWord();
        activeWords.push_back(Word{newWord, (float)(rand() % (SCREEN_WIDTH - MeasureText(newWord.c_str(), TEXT_SIZE))), 0.0f});
    }
    
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125)) {
            PlaySound(typingSound);
            SetSoundPitch(typingSound, 0.9f + (((float)rand() / RAND_MAX) * 0.2f));
            typedString += std::tolower((char)key);
        }
        key = GetCharPressed();
    }

    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_BACKSPACE)) {
        typedString.clear();
    }

    else if (IsKeyPressed(KEY_BACKSPACE) && !typedString.empty()) {
        typedString.pop_back();
    }

    currentMatches = 0;
    for(auto it = activeWords.begin(); it != activeWords.end(); ) {
        if(typedString.size() <= it->text.size() && (it->text.compare(0, typedString.length(), typedString) == 0)) {
            it->isPotentialMatched = true;
            currentMatches++;
            if (it->text == typedString) {
                it = activeWords.erase(it);
                score++;
                PlaySound(wordDestroyedSound);
                if (score > highScore){
                    highScore = score;
                }                
                currentMatches--;
                continue;
            }
        }
        else {
            if(it->isPotentialMatched){
                matchesReduced = true;
            }
            it->isPotentialMatched = false;
        }
        
        it->y += wordFallSpeed * dt;
        
        if (it->y > SCREEN_HEIGHT - (TEXT_SIZE*2) - 15) {
            if(it->isPotentialMatched) {
                currentMatches--;
            }
            it = activeWords.erase(it);
            
            lives--;
            if(lives <= 0) {
                saveHighScore();
                currentState = GameState::GAMEOVER;
                StopMusicStream(gameplayMusic);
                PlaySound(gameOverSound);
            }
            else{
                PlaySound(lifeLostSound);
                SetSoundPitch(lifeLostSound, 1.0f - (float(MAX_LIVES - lives) * 0.05f));
            }
        }
        else {
            ++it;
        }
    }

    if(currentMatches == 0) {
        typedString.clear();
        if(matchesReduced){
            if(!IsSoundPlaying(mistakeSound)){
                PlaySound(mistakeSound);
            }
        }
    }
        
    cursorBlinkTimer += dt;

    difficultyTimer += dt;
    if (difficultyTimer >= WORD_RATE_UPDATE_TIME) {
        difficultyTimer -= WORD_RATE_UPDATE_TIME;

        wordSpawnInterval *= 0.92f;
        if (wordSpawnInterval < 0.4f)
            wordSpawnInterval = 0.4f;

        wordFallSpeed += 9.0f;
    }

    for(int i = 0; i < MAX_LIVES; i++) {
        if (i >= lives) {
            heartAnims[i].update();
        }
    }
    if(lives == 1){
        heartAnims[0].update();
    }
}

void Game::resetGame() {
    wordSpawnTimer = 0.0f;
    difficultyTimer = 0.0f;
    wordSpawnInterval = 2.0f;
    wordFallSpeed = 80.0f;

    lives = MAX_LIVES;
    score = 0;
    activeWords.clear();
    typedString.clear();
    for(int i = 0; i < MAX_LIVES; i++) {
        heartAnims[i].cur = heartAnims[i].first;
    }
}

void Game::updatePause() {    
    UpdateMusicStream(gameplayMusic);

    if(IsKeyPressed(KEY_TAB)) {
        currentState = GameState::HOME;
        StopMusicStream(gameplayMusic);
        PlayMusicStream(mainMenuMusic);
        resetGame();
    }
    if(IsKeyPressed(KEY_SPACE)) {
        currentState = GameState::PLAY;      
        SetMusicVolume(gameplayMusic, 1.0f);  
    }
}

void Game::updateGameOver() {
    if(IsKeyPressed(KEY_ENTER)) {
        currentState = GameState::PLAY;
        SetMusicVolume(gameplayMusic, 1.0f);
        PlayMusicStream(gameplayMusic);
        resetGame();
    }

    if(IsKeyPressed(KEY_TAB)) {
        currentState = GameState::HOME;
        SetMusicVolume(mainMenuMusic, 1.0f);
        PlayMusicStream(mainMenuMusic);
        resetGame();
    }
}

void Game::draw() {
    switch (currentState)
    {
    case GameState::HOME:
        drawHome();
        break;
    
    case GameState::PLAY:
        drawPlay();
        break;
    
    case GameState::PAUSE:
        drawPause();
        break;
    
    case GameState::GAMEOVER:
        drawGameOver();
        break;
    
    default:
        break;
    }
}

void Game::drawHome() {
    const char* part1 = "WOR";
    const char* part2 = "D ";
    const char* part3 = "WOR";
    const char* part4 = "SHIP";
    
    int titleSize = TEXT_SIZE;
    int part1Width = MeasureText(part1, titleSize);
    int part2Width = MeasureText(part2, titleSize);
    int part3Width = MeasureText(part3, titleSize);
    int part4Width = MeasureText(part4, titleSize);
    int totalWidth = part1Width + part2Width + part3Width + part4Width;
    
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int titleY = SCREEN_HEIGHT / 2 - (titleSize * 2);
    
    DrawText(part1, startX, titleY, titleSize, SKYBLUE);
    DrawText(part2, startX + part1Width, titleY, titleSize, WHITE);
    DrawText(part3, startX + part1Width + part2Width, titleY, titleSize, SKYBLUE);
    DrawText(part4, startX + part1Width + part2Width + part3Width, titleY, titleSize, WHITE);
    
    int instructionSize = TEXT_SIZE / 2;
    const char* startText = "Press ENTER to Start";
    const char* exitText = "Press ESC to Exit";
    
    DrawText(startText, 
             (SCREEN_WIDTH - MeasureText(startText, instructionSize)) / 2, 
             SCREEN_HEIGHT / 2 + instructionSize, 
             instructionSize, 
             LIGHTGRAY);
    
    DrawText(exitText, 
             (SCREEN_WIDTH - MeasureText(exitText, instructionSize)) / 2, 
             SCREEN_HEIGHT / 2 + (instructionSize * 3), 
             instructionSize, 
             LIGHTGRAY);
}

void Game::drawPlay() {
    
    int typedTextWidth = MeasureText(typedString.c_str(), TEXT_SIZE);

    DrawRectangle(0, SCREEN_HEIGHT - TEXT_SIZE - 18, SCREEN_WIDTH, 3, RED);
    DrawRectangle(0, SCREEN_HEIGHT - TEXT_SIZE - 15, SCREEN_WIDTH, TEXT_SIZE + 15, GRAY);
    DrawText(typedString.c_str(), (SCREEN_WIDTH - typedTextWidth) / 2, SCREEN_HEIGHT - TEXT_SIZE - 5, TEXT_SIZE, GREEN);
    if (fmod(cursorBlinkTimer, 1.0f) < 0.5f) {
        DrawRectangle((SCREEN_WIDTH + typedTextWidth) / 2, SCREEN_HEIGHT - TEXT_SIZE - 5, 2, TEXT_SIZE, GREEN);
    }

    for (const Word& word : activeWords) {
        word.draw(typedString.length(), TEXT_SIZE);
    }

    const int BAR_HEIGHT = 40;
    DrawRectangle(0, 0, SCREEN_WIDTH, BAR_HEIGHT, BLUE);
    
    std::string scoreText = "Score: " + std::to_string(score);
    std::string highScoreText = "High Score: " + std::to_string(highScore);
    DrawText(scoreText.c_str(), 15, (BAR_HEIGHT - TEXT_SIZE + 10) / 2, TEXT_SIZE - 10, WHITE);
    DrawText(highScoreText.c_str(), 15 + MeasureText(scoreText.c_str(), TEXT_SIZE), (BAR_HEIGHT - TEXT_SIZE + 10) / 2, TEXT_SIZE - 10, WHITE);

    const int HEART_SIZE = 30;
    const int HEART_SPACING = 5;
    for(int i = 0; i < MAX_LIVES; i++) {
        float heartX = SCREEN_WIDTH - (MAX_LIVES - i) * (HEART_SIZE + HEART_SPACING) - 10;
        float heartY = (BAR_HEIGHT - HEART_SIZE) / 2;
        DrawTexturePro(heartTexture, heartAnims[i].frame(5), 
                       {heartX, heartY, HEART_SIZE, HEART_SIZE}, 
                       {0, 0}, 0.0f, WHITE);
    }
}

void Game::drawPause() {
    drawPlay();
    
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.9f));
    
    int instructionSize = TEXT_SIZE / 2;
    int titleY = SCREEN_HEIGHT / 2 - (TEXT_SIZE * 3);
    
    const char* part1 = "PAU";
    const char* part2 = "SED";
    
    int part1Width = MeasureText(part1, TEXT_SIZE);
    int part2Width = MeasureText(part2, TEXT_SIZE);
    int totalWidth = part1Width + part2Width;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    
    DrawText(part1, startX, titleY, TEXT_SIZE, SKYBLUE);
    DrawText(part2, startX + part1Width, titleY, TEXT_SIZE, WHITE);
    
    DrawText("Press SPACE to Resume", 
             (SCREEN_WIDTH - MeasureText("Press SPACE to Resume", instructionSize)) / 2, 
             SCREEN_HEIGHT / 2 + instructionSize, 
             instructionSize, 
             LIGHTGRAY);
    
    DrawText("Press TAB for Home Menu", 
             (SCREEN_WIDTH - MeasureText("Press TAB for Home Menu", instructionSize)) / 2, 
             SCREEN_HEIGHT / 2 + (instructionSize * 3), 
             instructionSize, 
             LIGHTGRAY);
}

void Game::drawGameOver() {
    int instructionSize = TEXT_SIZE / 2;
    int titleY = SCREEN_HEIGHT / 2 - (TEXT_SIZE * 4);
    
    const char* part1 = "GAME";
    const char* part2 = " OVER";
    
    int part1Width = MeasureText(part1, TEXT_SIZE);
    int part2Width = MeasureText(part2, TEXT_SIZE);
    int totalWidth = part1Width + part2Width;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    
    DrawText(part1, startX, titleY, TEXT_SIZE, SKYBLUE);
    DrawText(part2, startX + part1Width, titleY, TEXT_SIZE, WHITE);
    
    std::string highScoreText = "High Score: " + std::to_string(highScore);
    DrawText(highScoreText.c_str(), 
             (SCREEN_WIDTH - MeasureText(highScoreText.c_str(), instructionSize)) / 2, 
             SCREEN_HEIGHT / 2 - (TEXT_SIZE * 2), 
             instructionSize, 
             GOLD);
    
    std::string scoreText = "Your Score: " + std::to_string(score);
    DrawText(scoreText.c_str(), 
             (SCREEN_WIDTH - MeasureText(scoreText.c_str(), TEXT_SIZE)) / 2, 
             SCREEN_HEIGHT / 2 - TEXT_SIZE, 
             TEXT_SIZE, 
             SKYBLUE);
    
    DrawText("Press ENTER to Restart", 
             (SCREEN_WIDTH - MeasureText("Press ENTER to Restart", instructionSize)) / 2, 
             SCREEN_HEIGHT / 2 + (instructionSize * 2), 
             instructionSize, 
             LIGHTGRAY);
    
    DrawText("Press TAB for Home Menu", 
             (SCREEN_WIDTH - MeasureText("Press TAB for Home Menu", instructionSize)) / 2, 
             SCREEN_HEIGHT / 2 + (instructionSize * 4), 
             instructionSize, 
             LIGHTGRAY);
}