#include "SaveData.h"
#include <fstream>
#include <fstream>


uint32_t computeChecksum(const SaveData& data) {
    return (data.easy ^ data.medium ^ data.hard ^ data.texpert) + 0xFF40C811;
}

bool fileExists(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    return f.good();
}

bool loadHighScores(SaveData& outData, const std::string& path) {
    if(!fileExists(path)){
        outData = {0, 0, 0, 0, 0};
        return true;
    }

    std::ifstream in(path, std::ios::binary);
    if(!in){
        return false;
    }

    in.read(reinterpret_cast<char*>(&outData), sizeof(SaveData));
    if (!in) return false;

    return outData.checksum == computeChecksum(outData);
}

void saveHighScores(const SaveData& data, const std::string& path) {
    SaveData temp = data;
    temp.checksum = computeChecksum(temp);

    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(&temp), sizeof(SaveData));
}