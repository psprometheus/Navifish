#include <iostream>
#include <chrono>
#include "src/chess.hpp"

using namespace std;

uint64_t nodes = 0;

constexpr int rundepth = 7;

chess::Movelist movelists[rundepth];

void run(chess::Board &board, int depth) {
    nodes++;
    if (depth == 0) return;
    chess::Movelist ml;
    chess::movegen::legalmoves(ml, board);

    for (int i = 0; i < ml.size(); i++) {
        chess::Move move = ml[i];
        board.makeMove(move);
        run(board, depth - 1);
        board.unmakeMove(move);
    }
}

int main() {
    chess::Board board;
    auto start = chrono::high_resolution_clock::now();
    run(board, rundepth);
    auto end = chrono::high_resolution_clock::now();
    float duration = chrono::duration_cast<chrono::milliseconds>(end - start).count() / 1000.0f;

    cout << nodes << " " << duration << endl;
}
