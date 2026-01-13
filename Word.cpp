#include <string>
#include "Word.h"
#include <cmath>
#include <raylib.h>

Word::Word(const std::string& t, float xpos, float ypos) : text(t), x(xpos), y(ypos) {
}

void Word::draw(int matchedChars, int textSize) const{
    int gbComponent = 255 - (255* pow((y/GetScreenHeight()), 2));
    Color textColor = {255, gbComponent, gbComponent, 255};

    if(isPotentialMatched &&(matchedChars < text.size())) {
        std::string matchedText = text.substr(0, matchedChars);
        std::string unmatchedText = text.substr(matchedChars);
        int letterSpacing = MeasureText(text.c_str(), textSize) - (MeasureText(matchedText.c_str(), textSize) + MeasureText(unmatchedText.c_str(), textSize));
        
        DrawText(matchedText.c_str(), x, y, textSize, SKYBLUE);
        DrawText(unmatchedText.c_str(), x + MeasureText(matchedText.c_str(), textSize) + letterSpacing, y, textSize, textColor);
    }
    else{
        DrawText(text.c_str(), x, y, textSize, textColor);
    }
}