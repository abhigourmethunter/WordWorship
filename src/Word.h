#pragma once
#include <string>

class Word {
    public:
        Word(const std::string& t, float xpos, float ypos);
        void draw(int matchedChars, int textSize) const;

        std::string text;
        float x;
        float y;
        bool isPotentialMatched = false;
};