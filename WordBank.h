#pragma once
#include <string>
#include "Difficulty.h"

namespace WordBank {
    std::string getRandomWord();
    std::string getRandomWordWithDifficulty(Difficulty difficulty);
    void loadWords();
    void toLowerCase(std::string& str);
}
