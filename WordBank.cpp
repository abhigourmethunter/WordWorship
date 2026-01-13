#include "WordBank.h"
#include <vector>
#include <fstream>
#include <cstdlib>


namespace WordBank {
    static std::vector<std::vector<std::string>> words(20);

    void loadWords() {
        std::ifstream wordFile("assets/other/words.txt");
        std::string word;
        for(; std::getline(wordFile, word); ) {
            toLowerCase(word);
            words[word.size()].push_back(word);
        }
    }

    std::string getRandomWordWithDifficulty(Difficulty difficulty) {
        int limiter = 10;
        int offset = 0;
        switch(difficulty) {
            case EASY:
                {
                    limiter = 4;
                    break;
                }
            case MEDIUM:
                {
                    limiter = 10;
                    break;
                }
            case HARD:
                {
                    limiter = 16;
                    break;
                }
            case TEXPERT:
                {
                    limiter = 8;
                    offset = 8;
                    break;
                }


        }
        int numLetters = rand() % (limiter+1);
        if(words[numLetters + offset].empty()) {
            numLetters = 5;
            offset = 0;
        }
        return words[numLetters + offset][rand() % words[numLetters + offset].size()];
    }

    std::string getRandomWord() {
        int numLetters = rand() % 16;
        if(words[numLetters].empty()) {
            numLetters = 5;
        }

        return words[numLetters][rand() % words[numLetters].size()];
    }

    void toLowerCase(std::string& str) {
        for (char& c : str) {
            c = std::tolower(c);
        }
    }
}