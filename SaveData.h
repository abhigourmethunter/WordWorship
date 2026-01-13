#pragma once
#include <cstdint>
#include <string>

struct SaveData {
    int32_t easy;
    int32_t medium;
    int32_t hard;
    int32_t texpert;
    uint32_t checksum;
};

uint32_t computeChecksum(const SaveData& data);

bool loadHighScores(SaveData& outData, const std::string& path);
void saveHighScores(const SaveData& data, const std::string& path);