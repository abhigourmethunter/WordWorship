#pragma once
#include <string>

enum Difficulty {
    EASY,
    MEDIUM,
    HARD,
    TEXPERT
};


inline std::string to_string(Difficulty d) {
    switch (d) {
        case Difficulty::EASY:    return "EASY";
        case Difficulty::MEDIUM:  return "MEDIUM";
        case Difficulty::HARD:    return "HARD";
        case Difficulty::TEXPERT: return "TEXPERT";
    }
    return "UNKNOWN";
}
