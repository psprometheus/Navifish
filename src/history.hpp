#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>

#include "move.hpp"

using namespace std;

const int16_t HistoryInit = -30000;
const int16_t HistoryLimit = 30000;

struct History {
    inline static int16_t data[2][64][64];
    static void update(const chess::Move& move, int depth, bool sideToMove) {
        int16_t& score = data[sideToMove][move.from().index()][move.to().index()];
        score += depth * depth;
        score = min(score, HistoryLimit);
    }
    static void aging() {
        for (int s = 0; s < 2; s++) {
            for (int y = 0; y < 64; y++) {
                for (int x = 0; x < 64; x++) {
                    int16_t& score = data[s][y][x];
                    score = -30000 + (score + 30000) / 4;
                }
            }
        }
    }
    static void reset() {
        for (int s = 0; s < 2; s++) {
            for (int y = 0; y < 64; y++) {
                for (int x = 0; x < 64; x++) {
                    data[s][y][x] = HistoryInit;
                }
            }
        }
    }
};
