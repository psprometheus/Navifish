#pragma once

#include "move.hpp"
#include "board.hpp"

#define EXACT 0
#define UPPERBOUND 1
#define LOWERBOUND -1

struct TTEntry {
    uint64_t key = 0;
    float eval;
    chess::Move bestmove;
    int depth;
    int flag;
};

class TTTable {
public:
    static TTEntry get(const chess::Board& board) {
        uint64_t hash = board.hash();
        size_t index = hash & (slots - 1);
        return data[index];
    }
    static void add(const chess::Board& board, float eval, chess::Move bestmove, int depth, int flag) {
        uint64_t hash = board.hash();
        size_t index = hash & (slots - 1);
        TTEntry& entry = data[index];
        used += !entry.key;
        entry.key = hash;
        entry.eval = eval;
        entry.bestmove = bestmove;
        entry.depth = depth;
        entry.flag = flag;
    }
    static chess::Move get_bestmove(const chess::Board& board, int depth = 0) {
        uint64_t hash = board.hash();
        size_t index = hash & (slots - 1);
        TTEntry& entry = data[index];
        if (entry.key == hash && entry.depth >= depth)
            return entry.bestmove;
        else
            return chess::Move::NO_MOVE;
    }
    static int hashfull() {
        return 1000 * float(used) / slots;
    }
    static void free() {
        delete[] data;
    }

private:
    inline static const size_t slots = (1 << 20);
    inline static TTEntry* data = new TTEntry[slots];
    inline static size_t used = 0;
};