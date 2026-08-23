#include <iostream>
#include <limits.h>
#include <math.h>
#include <sstream>
#include <chrono>
#include <deque>
#include <signal.h>

#include "search.hpp"
#include "eval.hpp"

using namespace std;

void print_board(const chess::Board& board) {
    cout << board;
}

int main() {
    cout.setf(std::ios::unitbuf);

    HistoryReset();
    Eval::init_tables();
    init_lmr();

    string line, command;

    chess::Board board;

    while (getline(cin, line)) {
        stringstream ss(line);
        ss >> command;

        if (command == "uci") {
            cout << "id name SigmaChessZero" << endl;
            cout << "id author creper1337X" << endl;
            cout << "uciok" << endl;
        } else if (command == "isready") {
            cout << "readyok" << endl;
        } else if (command == "ucinewgame") {
            HistoryReset();
            board = chess::Board();
        } else if (command == "go") {
            ss >> command;
            int searchdepth = MAX_PLY;
            int movetime = INT_MAX;
            if (command == "depth") {
                ss >> command;
                searchdepth = stof(command);
            } else if (command == "movetime") {
                ss >> command;
                movetime = stof(command);
            }
            Search search(board, searchdepth, movetime);
        } else if (command == "position") {
            ss >> command;
            if (command == "fen") {
                string fen;
                while (ss >> command) {
                    fen += command + " ";
                }
                board.setFen(fen);
            } else if (command == "startpos") {
                board = chess::Board();
                ss >> command;
                while (ss >> command) {
                    chess::Move move = chess::uci::uciToMove(board, command);
                    board.makeMove(move);
                }
            }
        } else if (command == "quit") {
            break;
        }
    }

    TTTable::free();
    return 0;
}