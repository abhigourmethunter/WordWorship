#include "WordBank.h"
#include <vector>
#include <fstream>
#include <cstdlib>
#include <random>


namespace WordBank {
    static std::mt19937 gen((std::random_device{}()));
    static std::normal_distribution<float> dist(6, 2);
    static std::vector<std::vector<std::string>> words(30);

    static int MAX_WORD_LENGTH;
    
    void init() {
        std::ifstream wordFile("assets/other/words.txt");
        if (!wordFile) {
            throw std::runtime_error("Failed to open words.txt");
        }

        std::string word;
        int maxLength = 0;

        std::vector<std::pair<int, std::string>> temp;

        while (std::getline(wordFile, word)) {
            toLowerCase(word);
            if (word.empty()) continue;

            int len = static_cast<int>(word.size());
            maxLength = std::max(maxLength, len);

            temp.emplace_back(len, word);
        }

        MAX_WORD_LENGTH = maxLength;

  
        words.clear();
        words.resize(MAX_WORD_LENGTH + 1);

        for (auto& [len, w] : temp) {
            words[len].push_back(w);
        }
    }



    void setDistributionByDifficulty(Difficulty difficulty) {
        int mean = 6;
        int stddev = 2;
        switch(difficulty) {
            case Difficulty::EASY:
                mean = 3;
                stddev = 1;
                break;
            case Difficulty::MEDIUM:
                mean = 6;
                stddev = 2;
                break;
            case Difficulty::HARD:
                mean = 8;
                stddev = 3;
                break;
            case Difficulty::TEXPERT:
                mean = 15;
                stddev = 5;
                break;
        }
        dist = std::normal_distribution<float>(mean, stddev);
    }

    void loadWords() {
        std::ifstream wordFile("assets/other/words.txt");
        std::string word;
        for(; std::getline(wordFile, word); ) {
            toLowerCase(word);
            words[word.size()].push_back(word);
        }
    }

    std::string getRandomWordWithDifficulty() {
        int randomWordLength = static_cast<int>(std::round(dist(gen)));

        if(randomWordLength < 0) {
            randomWordLength = 0;
        }
        if(randomWordLength >= MAX_WORD_LENGTH) {
            randomWordLength = MAX_WORD_LENGTH;
        }
        if(words[randomWordLength].empty()) {
            randomWordLength = 5;
        }
        return words[randomWordLength][rand() % words[randomWordLength].size()];
    }

    std::string getRandomWord() {
        int numLetters = rand() % MAX_WORD_LENGTH;
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