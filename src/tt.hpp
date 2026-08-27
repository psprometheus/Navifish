#pragma once

#include "move.hpp"
#include "board.hpp"

#define EXACT 0
#define UPPERBOUND 1
#define LOWERBOUND -1

struct TTEntry {
    uint64_t key;
    int16_t eval;
    uint16_t bestmove;
    uint8_t depth = 0;
    int8_t flag;
    uint16_t age = 0;
};

struct alignas(64) Bucket{
    TTEntry entries[4];
};

class TTTable {
public:
    static TTEntry get(const chess::Board& board) {
        uint64_t hash = board.hash();
        size_t index = hash & (slots - 1);
        Bucket& bucket = data[index];
        for (int i = 0; i < 4; i++) {
            TTEntry& entry = bucket.entries[i];
            if (entry.key == hash)
                return entry;
        }
        return TTEntry();
    }
    static void overwrite(TTEntry& entry, uint64_t key, float eval, chess::Move bestmove, int depth, int flag) {
        used += !entry.depth;
        entry.key = key;
        entry.eval = eval;
        entry.bestmove = bestmove.move();
        entry.depth = depth;
        entry.flag = flag;
        entry.age = 0;
    }
    static void add(const chess::Board& board, float eval, chess::Move bestmove, int depth, int flag) {
        uint64_t hash = board.hash();
        size_t index = hash & (slots - 1);
        Bucket& bucket = data[index];
        for (auto& entry : bucket.entries) {
            if (entry.key == hash) {
                if (entry.depth == depth && abs(entry.flag) >= abs(flag)) {
                    overwrite(entry, hash, eval, bestmove, depth, flag);
                }
                if (entry.depth < depth) {
                    overwrite(entry, hash, eval, bestmove, depth, flag);
                }
                return;
            }
        }
        int replace = 0;
        int min_score = INT_MAX;
        for (int i = 0; i < 4; i++) {
            TTEntry& entry = bucket.entries[i];
            int punished_score = 2 * entry.depth - 5 * entry.age;
            if (entry.depth == 0) {
                overwrite(entry, hash, eval, bestmove, depth, flag);
                return;
            }
            if (punished_score < min_score) {
                replace = i;
                min_score = punished_score;
            }
        }
        TTEntry& entry = bucket.entries[replace];
        overwrite(entry, hash, eval, bestmove, depth, flag);
    }
    static chess::Move get_bestmove(const chess::Board& board, int depth = 0) {
        uint64_t hash = board.hash();
        size_t index = hash & (slots - 1);
        Bucket& bucket = data[index];
        for (int i = 0; i < 4; i++) {
            const TTEntry& entry = bucket.entries[i];
            if (entry.key == hash) {
                if (entry.depth >= depth)
                    return entry.bestmove;
            }
        }
        return chess::Move::NO_MOVE;
    }
    static void aging() {
        for (int i = 0; i < slots; i++) {
            Bucket& bucket = data[i];
            for (auto& entry : bucket.entries) {
                entry.age++;
            }
        }
    }
    static int hashfull() {
        return 1000 * float(used) / (slots * 4);
    }
    static void free() {
        delete[] data;
    }

private:
    inline static const size_t slots = (1 << 18);
    inline static Bucket* data = new Bucket[slots];
    inline static size_t used = 0;
};