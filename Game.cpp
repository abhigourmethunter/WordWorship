#include "Game.h"
#include "Word.h"
#include "WordBank.h"
#include <cmath>
#include <fstream>
#include <iostream>

Game::Game(const int screen_width, const int screen_height)
    : SCREEN_WIDTH(screen_width),
      SCREEN_HEIGHT(screen_height),
      DIFFICULTY(Difficulty::MEDIUM),
      wordSpawnTimer(0.0f),
      difficultyTimer(0.0f),
      wordSpawnInterval(3.0f),
      wordFallSpeed(80.0f),
      redFlashCounter(0),
      redFlash(false),
      firstPlay(false),
      minusPressedCounter(0),
      plusPressedCounter(0) {

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
    bool flag = loadHighScores(highScores, HIGH_SCORE_FILE);
    if(!flag){
        std::cout << "failed to load highscores";
    }
}

void Game::saveHighScore() {
    saveHighScores(highScores, HIGH_SCORE_FILE);
}

int Game::getHighScoreForDiffLevel(){
    switch(DIFFICULTY){
        case Difficulty::EASY:
            return highScores.easy;
        case Difficulty::MEDIUM:
            return highScores.medium;
        case Difficulty::HARD:
            return highScores.hard;
        case Difficulty::TEXPERT:
            return highScores.texpert;
        default:
            return -1;
    }
    return -1;
}

void Game::setHighScoreForDiffLevel(int score){
    switch(DIFFICULTY){
        case Difficulty::EASY:
            highScores.easy = score;
            break;
        case Difficulty::MEDIUM:
            highScores.medium = score;
            break;
        case Difficulty::HARD:
            highScores.hard = score;
            break;
        case Difficulty::TEXPERT:
            highScores.texpert = score;
            break;
        default:
            std::cout << "what da hael";
    }
}

void Game::update() {
    if (IsKeyPressed(KEY_EQUAL)) {
        plusPressedCounter = 6;
        PlaySound(clickSound);
        float vol = GetMasterVolume();
        if(vol >= 1.0f){
            vol = 1.0f;
        }
        else{
            vol += 0.1f;
        }
        SetMasterVolume(vol);
    }else{
        plusPressedCounter--;
    }
    if (IsKeyPressed(KEY_MINUS)) {
        minusPressedCounter = 6;
        PlaySound(clickSound);
        float vol = GetMasterVolume();
        if(vol <= 0.0f){
            vol = 0.0f;
        }
        else{
            vol -= 0.1f;
        }
        SetMasterVolume(vol);
    }else{
        minusPressedCounter--;
    }

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
    int difficultySetter = DIFFICULTY + 1;

    if(IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)){
        difficultySetter = 1;
        PlaySound(clickSound);
    }
    else if(IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)){
        difficultySetter = 2;
        PlaySound(clickSound);
    }
    else if(IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)){
        difficultySetter = 3;
        PlaySound(clickSound);
    }
    else if(IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)){
        difficultySetter = 4;
        PlaySound(clickSound);
    }
    else if(IsKeyPressed(KEY_RIGHT)){
        PlaySound(clickSound);
        difficultySetter++;
        if(difficultySetter > 4){
            difficultySetter = 1;
        }
    }
    else if(IsKeyPressed(KEY_LEFT)){
        PlaySound(clickSound);
        difficultySetter--;
        if(difficultySetter < 1){
            difficultySetter = 4;
        }
    }

    switch(difficultySetter){
        case 1:
            DIFFICULTY = Difficulty::EASY;
            break;
        case 2:
            DIFFICULTY = Difficulty::MEDIUM;
            break;
        case 3:
            DIFFICULTY = Difficulty::HARD;
            break;
        case 4:
            DIFFICULTY = Difficulty::TEXPERT;
            break;
        default:
            DIFFICULTY = Difficulty::MEDIUM;
            break;
    }

    UpdateMusicStream(mainMenuMusic);

    if(IsKeyPressed(KEY_ENTER)) {
        WordBank::setDistributionByDifficulty(DIFFICULTY);
        if(getHighScoreForDiffLevel() == 0){
            firstPlay = true;
        }
        PlaySound(clickSound);
        currentState = GameState::PLAY;
        StopMusicStream(mainMenuMusic);
        PlayMusicStream(gameplayMusic);
    }
}

void Game::loadAudios() {
    clickSound = LoadSound("assets/audio/sound_effects/click.wav");
    wordDestroyedSound = LoadSound("assets/audio/sound_effects/word_destroyed.wav");
    mistakeSound = LoadSound("assets/audio/sound_effects/mistake.wav");
    lifeLostSound = LoadSound("assets/audio/sound_effects/lose_life.wav");
    gameOverSound = LoadSound("assets/audio/sound_effects/game_over.wav");
    typingSound = LoadSound("assets/audio/sound_effects/typing.wav");
    mainMenuMusic = LoadMusicStream("assets/audio/music/menu_music.wav");
    gameplayMusic = LoadMusicStream("assets/audio/music/gameplay_music.wav");
    newHighScoreSound = LoadSound("assets/audio/sound_effects/new_highscore.wav");
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
        PlaySound(clickSound);
        currentState = GameState::PAUSE;
        SetMusicVolume(gameplayMusic, 0.2f);
        return;
    }

    wordSpawnTimer += dt;
    if (wordSpawnTimer >= wordSpawnInterval) {
        wordSpawnTimer = 0.0f;

        std::string newWord = WordBank::getRandomWordWithDifficulty();
        activeWords.push_back(Word{newWord, (float)(rand() % (SCREEN_WIDTH - MeasureText(newWord.c_str(), TEXT_SIZE))), 0.0f});
    }
    
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {
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
                if ((score > getHighScoreForDiffLevel())){
                    if(!newHighScoreFlag && !firstPlay){
                        newHighScoreFlag = true;
                        PlaySound(newHighScoreSound);
                        highScoreTimer = highScoreDuration;
                    }
                    setHighScoreForDiffLevel(score);
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
                beat = 60 / (MAX_LIVES - lives + 1);
                StopMusicStream(gameplayMusic);
                PlaySound(gameOverSound);
            }
            else{
                PlaySound(lifeLostSound);
                redFlash = false;
                redFlashCounter = 60;
                beat = 60 / (((MAX_LIVES - lives) * 2) - 1);
                SetSoundPitch(lifeLostSound, 1.0f - (float(MAX_LIVES - lives) * 0.05f));
            }
        }
        else {
            ++it;
        }

    }

    if(redFlashCounter){
        if(redFlashCounter % beat == 0){
            redFlash = !redFlash;
        }
        redFlashCounter--;
    }

    if(highScoreTimer > 0) {
        newHighScore();
        highScoreTimer -= dt;
        
        if(highScoreTimer <= 0) {
            multiplierForNewHighscore = 1.0f;
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
    wordSpawnInterval = 2.5f;
    wordFallSpeed = 80.0f;

    lives = MAX_LIVES;
    score = 0;
    activeWords.clear();
    typedString.clear();
    for(int i = 0; i < MAX_LIVES; i++) {
        heartAnims[i].cur = heartAnims[i].first;
    }
    newHighScoreFlag = false;
    highScoreTimer = 0.0f;
    multiplierForNewHighscore = 1.0f;
}

void Game::updatePause() {    
    UpdateMusicStream(gameplayMusic);

    if(IsKeyPressed(KEY_TAB)) {
        PlaySound(clickSound);
        saveHighScore();
        currentState = GameState::HOME;
        SetMusicVolume(gameplayMusic, 1.0f);
        StopMusicStream(gameplayMusic);
        PlayMusicStream(mainMenuMusic);
        resetGame();
    }
    if(IsKeyPressed(KEY_ENTER)) {
        PlaySound(clickSound);
        saveHighScore();
        currentState = GameState::PLAY;
        SetMusicVolume(gameplayMusic, 1.0f);
        StopMusicStream(gameplayMusic);
        PlayMusicStream(gameplayMusic);
        resetGame();
    }
    if(IsKeyPressed(KEY_SPACE)) {
        PlaySound(clickSound);
        currentState = GameState::PLAY;      
        SetMusicVolume(gameplayMusic, 1.0f);  
    }
}

void Game::updateGameOver() {
    if(IsKeyPressed(KEY_ENTER)) {
        PlaySound(clickSound);
        currentState = GameState::PLAY;
        PlayMusicStream(gameplayMusic);
        resetGame();
    }

    if(IsKeyPressed(KEY_TAB)) {
        PlaySound(clickSound);
        currentState = GameState::HOME;
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

    drawVolumeBar();
}

void Game::drawHome() {
    const char* part1 = "WOR";
    const char* part2 = "D ";
    const char* part3 = "WOR";
    const char* part4 = "SHIP";
    
    int part1Width = MeasureText(part1, TITLE_SIZE);
    int part2Width = MeasureText(part2, TITLE_SIZE);
    int part3Width = MeasureText(part3, TITLE_SIZE);
    int part4Width = MeasureText(part4, TITLE_SIZE);
    int totalWidth = part1Width + part2Width + part3Width + part4Width;
    
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int titleY = SCREEN_HEIGHT / 2 - (TITLE_SIZE * 3);
    int letterSpacing = findLetterSpacing(TITLE_SIZE);
    
    DrawText(part1, startX, titleY, TITLE_SIZE, SKYBLUE);
    DrawText(part2, startX + part1Width + letterSpacing, titleY, TITLE_SIZE, WHITE);
    DrawText(part3, startX + part1Width + part2Width + (letterSpacing * 2), titleY, TITLE_SIZE + 1, SKYBLUE);
    DrawText(part4, startX + part1Width + part2Width + part3Width + (letterSpacing * 3), titleY, TITLE_SIZE, WHITE);
    
    const char* startText = "Press  [ENTER]  to Start";
    const char* exitText = "Press  [ESC]  to Exit";
    
    DrawText(startText, 
             (SCREEN_WIDTH - MeasureText(startText, INSTRUCTION_SIZE)) / 2, 
             titleY + TITLE_SIZE + (INSTRUCTION_SIZE * 2), 
             INSTRUCTION_SIZE, 
             LIGHTGRAY);
    
    DrawText(exitText, 
             (SCREEN_WIDTH - MeasureText(exitText, INSTRUCTION_SIZE)) / 2, 
             titleY + TITLE_SIZE + (INSTRUCTION_SIZE * 3.5), 
             INSTRUCTION_SIZE, 
             LIGHTGRAY);
    Difficulty currentDifficulty = DIFFICULTY;
    
    const char* difficultyLabel = "Select Difficulty:";
    const char* difficulties[] = {"Easy (1)", "Medium (2)", "Hard (3)", "Texpert (4)"};
    
    int bottomY = SCREEN_HEIGHT - (INSTRUCTION_SIZE * 7);
    int difficultyLabelSize = INSTRUCTION_SIZE*1.25;

    DrawText(difficultyLabel, 
             (SCREEN_WIDTH - MeasureText(difficultyLabel, difficultyLabelSize)) / 2, 
             bottomY - INSTRUCTION_SIZE * 2, 
             difficultyLabelSize, 
             LIGHTGRAY);
    
    int spacing = 40;
    int totalDiffWidth = 0;
    for (int i = 0; i < 4; i++) {
        totalDiffWidth += MeasureText(difficulties[i], INSTRUCTION_SIZE);
    }
    totalDiffWidth += spacing * 3;
    
    int startDiffX = (SCREEN_WIDTH - totalDiffWidth) / 2;
    int currentX = startDiffX;
    
    for (int i = 0; i < 4; i++) {
        Color color = (currentDifficulty == i) ? WHITE : GRAY;
        DrawText(difficulties[i], currentX, bottomY, INSTRUCTION_SIZE, color);
        currentX += MeasureText(difficulties[i], INSTRUCTION_SIZE) + spacing;
    }

    int highScore = getHighScoreForDiffLevel();
    std::string highScoreText = "High Score: " + std::to_string(highScore);
    DrawText(highScoreText.c_str(),
             (SCREEN_WIDTH - MeasureText(highScoreText.c_str(), INSTRUCTION_SIZE)) / 2,
             bottomY + INSTRUCTION_SIZE * 3,
             INSTRUCTION_SIZE,
             GOLD);
}

void Game::drawPlay() {

    std::string pauseText = "[SPACE] to pause";
    std::string difficultyText = "Difficulty: " + to_string(DIFFICULTY);

    float pauseSize = INSTRUCTION_SIZE * 1.15f;
    float diffSize  = INSTRUCTION_SIZE * 1.15f;

    float spacing = INSTRUCTION_SIZE * 0.5f;

    float totalHeight = pauseSize + spacing + diffSize;

    float yTop = (SCREEN_HEIGHT - totalHeight) / 2;

    DrawText(pauseText.c_str(),
            (SCREEN_WIDTH - MeasureText(pauseText.c_str(), pauseSize)) / 2,
            yTop,
            pauseSize,
            {200, 200, 200, 70});

    DrawText(difficultyText.c_str(),
            (SCREEN_WIDTH - MeasureText(difficultyText.c_str(), diffSize)) / 2,
            yTop + pauseSize + spacing,
            diffSize,
            {200, 200, 200, 70});

    
    int typedTextWidth = MeasureText(typedString.c_str(), TEXT_SIZE);
    DrawRectangle(0, SCREEN_HEIGHT - TEXT_SIZE - 18, SCREEN_WIDTH, 3, RED);
    DrawRectangle(0, SCREEN_HEIGHT - TEXT_SIZE - 15, SCREEN_WIDTH, TEXT_SIZE + 15, GRAY);
    DrawText(typedString.c_str(), (SCREEN_WIDTH - typedTextWidth) / 2, SCREEN_HEIGHT - TEXT_SIZE - 5, TEXT_SIZE, typedStringCOlor);
    if (fmod(cursorBlinkTimer, 1.0f) < 0.5f) {
        DrawRectangle((SCREEN_WIDTH + typedTextWidth) / 2, SCREEN_HEIGHT - TEXT_SIZE - 5, 2, TEXT_SIZE, typedStringCOlor);
    }
    for (const Word& word : activeWords) {
        word.draw(typedString.length(), TEXT_SIZE);
    }
    const int BAR_HEIGHT = 40;
    DrawRectangle(0, 0, SCREEN_WIDTH, BAR_HEIGHT, PURPLE);
    Color scoreColor = {60, 30, 60, 255};
    Color highScoreColor = {185, 108, 0, 255};
    int scoreSize = SCORE_SIZE * multiplierForNewHighscore;
    std::string highScoreText = "High Score: " + std::to_string(getHighScoreForDiffLevel());
    DrawText(highScoreText.c_str(), 15, (BAR_HEIGHT - scoreSize) / 2, scoreSize, highScoreColor);
    const int HEART_SIZE = 30;
    const int HEART_SPACING = 5;
    for(int i = 0; i < MAX_LIVES; i++) {
        float heartX = SCREEN_WIDTH - (MAX_LIVES - i) * (HEART_SIZE + HEART_SPACING) - 10;
        float heartY = (BAR_HEIGHT - HEART_SIZE) / 2;
        DrawTexturePro(heartTexture, heartAnims[i].frame(5), 
                       {heartX, heartY, HEART_SIZE, HEART_SIZE}, 
                       {0, 0}, 0.0f, WHITE);
    }
    std::string scoreText = "Score: " + std::to_string(score);
    int highScoreTextWidth = MeasureText(highScoreText.c_str(), scoreSize);
    int scoreTextWidth = MeasureText(scoreText.c_str(), scoreSize);
    int leftEdge = 15 + highScoreTextWidth;
    int rightEdge = SCREEN_WIDTH - (MAX_LIVES * (HEART_SIZE + HEART_SPACING)) - 10;
    int centerX = (leftEdge + rightEdge) / 2 - scoreTextWidth / 2;

    DrawText(scoreText.c_str(), centerX, (BAR_HEIGHT - scoreSize) / 2, scoreSize, (newHighScoreFlag)?highScoreColor:scoreColor);

    if(redFlashCounter){
        drawFlash();
    }
}

void Game::drawPause() {
    drawPlay();
    
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.9f));
    
    int titleY = SCREEN_HEIGHT / 2 - (TEXT_SIZE * 3);
    
    const char* part1 = "PAU";
    const char* part2 = "SED";
    
    int part1Width = MeasureText(part1, TITLE_SIZE);
    int part2Width = MeasureText(part2, TITLE_SIZE);
    int totalWidth = part1Width + part2Width;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    
    int letterSpacing = findLetterSpacing(TITLE_SIZE);
    DrawText(part1, startX, titleY, TITLE_SIZE, SKYBLUE);
    DrawText(part2, startX + part1Width + letterSpacing, titleY, TITLE_SIZE, WHITE);

    std::string scoreText = "Score: " + std::to_string(score);
    DrawText(scoreText.c_str(), 
             (SCREEN_WIDTH - MeasureText(scoreText.c_str(), INSTRUCTION_SIZE)) / 2, 
             titleY + TITLE_SIZE + INSTRUCTION_SIZE, 
             INSTRUCTION_SIZE, 
             WHITE);

    std::string highScoreText = "High Score: " + std::to_string(getHighScoreForDiffLevel());
    DrawText(highScoreText.c_str(), 
             (SCREEN_WIDTH - MeasureText(highScoreText.c_str(), INSTRUCTION_SIZE)) / 2, 
             titleY + TITLE_SIZE + (INSTRUCTION_SIZE * 2.5), 
             INSTRUCTION_SIZE, 
             GOLD);

    DrawText("Press  [SPACE]  to Resume",
             (SCREEN_WIDTH - MeasureText("Press [SPACE] to Resume", INSTRUCTION_SIZE)) / 2, 
             SCREEN_HEIGHT / 2 + (INSTRUCTION_SIZE * 2), 
             INSTRUCTION_SIZE, 
             LIGHTGRAY);
    
    DrawText("Press  [TAB]  for Home Menu", 
             (SCREEN_WIDTH - MeasureText("Press [TAB] for Home Menu", INSTRUCTION_SIZE)) / 2, 
             SCREEN_HEIGHT / 2 + (INSTRUCTION_SIZE * 3.5), 
             INSTRUCTION_SIZE, 
             LIGHTGRAY);
    
    DrawText("Press  [ENTER]  to Restart", 
             (SCREEN_WIDTH - MeasureText("Press  [ENTER]  to Restart", INSTRUCTION_SIZE)) / 2, 
             SCREEN_HEIGHT / 2 + (INSTRUCTION_SIZE * 5), 
             INSTRUCTION_SIZE, 
             LIGHTGRAY);
}

void Game::drawGameOver() {
    int titleY = SCREEN_HEIGHT / 2 - (TEXT_SIZE * 3.5);
    
    const char* part1 = "GA";
    const char* part2 = "ME ";
    const char* part3 = "OVER";
    
    int part1Width = MeasureText(part1, TITLE_SIZE);
    int part2Width = MeasureText(part2, TITLE_SIZE);
    int part3Width = MeasureText(part3, TITLE_SIZE);
    int totalWidth = part1Width + part2Width + part3Width;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    
    int letterSpacing = findLetterSpacing(TITLE_SIZE);
    
    DrawText(part1, startX, titleY, TITLE_SIZE, SKYBLUE);
    DrawText(part2, startX + part1Width + letterSpacing, titleY, TITLE_SIZE, WHITE);
    DrawText(part3, startX + part1Width + part2Width + (letterSpacing * 2), titleY, TITLE_SIZE, {255, 0, 0, 255});
    
    int scoreTextSize = INSTRUCTION_SIZE * 1.5;

    std::string scoreText = "Score: " + std::to_string(score);
    DrawText(scoreText.c_str(), 
             (SCREEN_WIDTH - MeasureText(scoreText.c_str(), scoreTextSize)) / 2, 
             titleY + TITLE_SIZE + (scoreTextSize * 1.5), 
             scoreTextSize, 
             WHITE);

    std::string highScoreText = "High Score: " + std::to_string(getHighScoreForDiffLevel());
    DrawText(highScoreText.c_str(), 
             (SCREEN_WIDTH - MeasureText(highScoreText.c_str(), scoreTextSize)) / 2, 
             titleY + TITLE_SIZE + (scoreTextSize * 3), 
             scoreTextSize, 
             GOLD);

    DrawText("Press  [ENTER]  to Restart", 
             (SCREEN_WIDTH - MeasureText("Press  [ENTER]  to Restart", INSTRUCTION_SIZE)) / 2, 
             SCREEN_HEIGHT / 2 + (INSTRUCTION_SIZE * 3.5), 
             INSTRUCTION_SIZE, 
             LIGHTGRAY);
    
    DrawText("Press  [TAB]  for Home Menu", 
             (SCREEN_WIDTH - MeasureText("Press  [TAB]  for Home Menu", INSTRUCTION_SIZE)) / 2, 
             SCREEN_HEIGHT / 2 + (INSTRUCTION_SIZE * 5), 
             INSTRUCTION_SIZE, 
             LIGHTGRAY);
}

void Game::drawFlash() {
    if(redFlash){
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {255, 0, 0, 100});
    }
    else{
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {255, 0, 0, 35});
    }
}

void Game::drawVolumeBar(){
    Color minusColor = minusPressedCounter > 0 ? SKYBLUE : WHITE;
    Color plusColor = plusPressedCounter > 0 ? SKYBLUE : WHITE;
    
    DrawText("[-]", SCREEN_WIDTH - 170, SCREEN_HEIGHT - 35, 20, minusColor);
    
    for(int i = 0; i < 10; i++){
        Color barColor = (i < (int)(GetMasterVolume() * 10)) ? SKYBLUE : DARKGRAY;
        DrawRectangle(SCREEN_WIDTH - 145 + i * 11, SCREEN_HEIGHT - 40, 10, 30, barColor);
    }
    
    DrawText("[+]", SCREEN_WIDTH - 145 + 10 * 11 + 5, SCREEN_HEIGHT - 35, 20, plusColor);
}

int Game::findLetterSpacing(int fontSize){
    return MeasureText("AB", fontSize) - (MeasureText("A", fontSize) + MeasureText("B", fontSize));
}

void Game::newHighScore(){
    float progress = highScoreTimer / highScoreDuration;
    multiplierForNewHighscore = 1.0f + 0.3f * sin(progress * 3.14159f * 6);
}