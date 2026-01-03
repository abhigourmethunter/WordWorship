#include <string>
#include "Word.h"
#include <cmath>
#include <raylib.h>

Word::Word(const std::string& t, float xpos, float ypos) : text(t), x(xpos), y(ypos) {
}

void Word::draw(int matchedChars, int textSize) const{
    int gbComponent = 255 - (255* pow((y/GetScreenHeight()), 2));
    Color textColor = {255, gbComponent, gbComponent, 255};

    if(isPotentialMatched) {
        DrawText(text.substr(0, matchedChars).c_str(), x, y, textSize, SKYBLUE);
        DrawText(text.substr(matchedChars).c_str(), x + MeasureText(text.substr(0, matchedChars).c_str(), textSize) + 3, y, textSize, textColor);
    }
    else{
        DrawText(text.c_str(), x, y, textSize, textColor);
    }
}