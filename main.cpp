#include <iostream>
#include <raylib.h>
#include "WordBank.h"
#include "Game.h"

int main () {

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Word Worship");
   
    Image icon = LoadImage("assets/art/icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetTargetFPS(60);

    InitAudioDevice();

    WordBank::init();
    Game game = Game(SCREEN_WIDTH, SCREEN_HEIGHT);

    if(!IsAudioDeviceReady()){
        std::cerr << "Audio Device Not Ready!" << std::endl;
        return -1;
    }
    
    SetMasterVolume(0.7f);

    while (WindowShouldClose() == false){


        game.update();
        
        BeginDrawing();
            ClearBackground(BLACK);
            game.draw();
        EndDrawing();

    }

    CloseAudioDevice();

    CloseWindow();

}