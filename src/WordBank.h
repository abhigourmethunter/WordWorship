#pragma once

#include <string>
#include "Difficulty.h"

namespace WordBank {
    
    void init();
    void loadWords();
    void setDistributionByDifficulty(Difficulty difficulty);
    std::string getRandomWordWithDifficulty();
    std::string getRandomWord();
    void toLowerCase(std::string& str);

}
