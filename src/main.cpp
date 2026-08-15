#include <iostream>
#include <limits.h>
#include <math.h>
#include <sstream>
#include <chrono>
#include <deque>
#include "chess.hpp"
using namespace std;

#define PAWN   0
#define KNIGHT 1
#define BISHOP 2
#define ROOK   3
#define QUEEN  4
#define KING   5

#define WHITE  0
#define BLACK  1

#define FLIP(sq) ((sq)^56)
#define OTHER(side) ((side)^ 1)

int mg_value[6] = { 82, 337, 365, 477, 1025,  0};
int eg_value[6] = { 94, 281, 297, 512,  936,  0};

int mg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

int eg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

int mg_knight_table[64] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

int eg_knight_table[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

int mg_bishop_table[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

int eg_bishop_table[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

int mg_rook_table[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

int eg_rook_table[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

int mg_queen_table[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

int eg_queen_table[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

int mg_king_table[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

int eg_king_table[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

int* mg_pesto_table[6] =
{
    mg_pawn_table,
    mg_knight_table,
    mg_bishop_table,
    mg_rook_table,
    mg_queen_table,
    mg_king_table
};

int* eg_pesto_table[6] =
{
    eg_pawn_table,
    eg_knight_table,
    eg_bishop_table,
    eg_rook_table,
    eg_queen_table,
    eg_king_table
};

int gamephaseInc[12] = {0,1,1,2,4,0,0,1,1,2,4,0};
int mg_table[12][64];
int eg_table[12][64];

void init_tables()
{
    int pc, p, sq;
    for (p = PAWN, pc = 0; p <= KING; pc++, p++) {
        for (sq = 0; sq < 64; sq++) {
            mg_table[pc]  [sq] = mg_value[p] + mg_pesto_table[p][sq];
            eg_table[pc]  [sq] = eg_value[p] + eg_pesto_table[p][sq];
            mg_table[pc+6][sq] = mg_value[p] + mg_pesto_table[p][FLIP(sq)];
            eg_table[pc+6][sq] = eg_value[p] + eg_pesto_table[p][FLIP(sq)];
        }
    }
}

int eval(const chess::Board& board)
{
    int mg[2];
    int eg[2];
    int gamePhase = 0;

    mg[WHITE] = 0;
    mg[BLACK] = 0;
    eg[WHITE] = 0;
    eg[BLACK] = 0;

    uint64_t bb = board.occ().getBits();

    while (bb) {
        int sq = __builtin_ctzll(bb);
        bb &= bb - 1;
        chess::Piece piece = board.at(sq);
        mg[piece.color()] += mg_table[piece][FLIP(sq)];
        eg[piece.color()] += eg_table[piece][FLIP(sq)];
        gamePhase += gamephaseInc[piece];

    }

    int mgScore = mg[board.sideToMove()] - mg[OTHER(board.sideToMove())];
    int egScore = eg[board.sideToMove()] - eg[OTHER(board.sideToMove())];
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24;
    int egPhase = 24 - mgPhase;
    float eval = (mgScore * mgPhase + egScore * egPhase) / 24;
    return (board.sideToMove() ? -eval : eval);
}

void print_board(const chess::Board& board) {
    cout << board;
}

constexpr int nodespercheck = (1 << 11) - 1;

constexpr int EXACT = 0;
constexpr int UPPERBOUND = 1;
constexpr int LOWERBOUND = -1;

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
    static void free() {
        delete[] data;
    }

private:
    inline static const size_t slots = (1 << 20);
    inline static TTEntry* data = new TTEntry[slots];
};

bool silence_move(chess::Board& board, const chess::Move& move) {
    if (board.at(move.to()) != chess::Piece::NONE)
        return 0;
    if (move.typeOf() == chess::Move::PROMOTION || move.typeOf() == chess::Move::ENPASSANT)
        return 0;
    if (board.givesCheck(move) != chess::CheckType::NO_CHECK)
        return 0;
    return 1;
}

constexpr float drawfactor = -0.1;

constexpr float delta = 50;
constexpr float R = 2;
constexpr float LMR_Scaling = 5;
constexpr float LMR_Base = 0;

constexpr int MAX_PLY = 128;

chess::Move KillerMoves[MAX_PLY][2];

int LMRTable[128][255] = {0};

void init_lmr() {
    for (int depth = 1; depth < 128; depth++) {
        for (int movecount = 1; movecount < 255; movecount++) {
                LMRTable[depth][movecount] = std::round(LMR_Base + log(depth) * log(movecount) / LMR_Scaling);
        }
    }
}

int16_t ButterflyHeuristic[64][64] = {0};

constexpr int16_t TTMoveScore = 32767;
constexpr int16_t PVMoveScore = 32766;
constexpr int16_t CaptureBase = 32700;
constexpr int16_t HistoryInit = -30000;
constexpr int16_t HistoryLimit = 30000;
constexpr int16_t KillerMove1Score = CaptureBase - 50;
constexpr int16_t KillerMove2Score = CaptureBase - 100;

int16_t MVV_LVA[7][6] = {
    {15, 14, 13, 12, 11, 10}, // P
    {25, 24, 23, 22, 21, 20}, // N
    {35, 34, 33, 32, 31, 30}, // B
    {45, 44, 43, 42, 41, 40}, // R
    {55, 54, 53, 52, 51, 50}, // Q
    {0 , 0 , 0 , 0 , 0 , 0 }, // NONE
    {0 , 0 , 0 , 0 , 0 , 0 }, // NONE
//   P , N , B , R , Q , K
};

void HistoryReset() {
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            ButterflyHeuristic[y][x] = HistoryInit;
        }
    }
}

void move_valuing(chess::Movelist& movelist, chess::Board& board, const chess::Move& pvmove, int depth = 0, int ply = MAX_PLY - 1) {
    chess::Move ttmove = TTTable::get_bestmove(board, depth);
    for (int i = 0; i < movelist.size(); i++) {
        chess::Move& move = movelist[i];
        if (move == ttmove) {
            move.setScore(TTMoveScore);
            continue;
        }
        if (move == pvmove) {
            move.setScore(PVMoveScore);
            continue;
        }
        if (move.typeOf() == chess::Move::ENPASSANT) {
            move.setScore(CaptureBase + 15);
            continue;
        }
        chess::PieceType ptfrom = board.at(move.from()).type();
        chess::PieceType ptto = board.at(move.to()).type();
        if (silence_move(board, move)) {
            if (move == KillerMoves[ply][0]) {
                move.setScore(KillerMove1Score);
                continue;
            } else if (move == KillerMoves[ply][1]) {
                move.setScore(KillerMove2Score);
                continue;
            }
            int16_t score = ButterflyHeuristic[move.from().index()][move.to().index()];
            move.setScore(score);
        } else {
            int16_t score = MVV_LVA[ptto][ptfrom];
            move.setScore(CaptureBase + score);
        }
    }
}

void search(chess::Board& board, int search_depth, int movetime) {
    auto start = chrono::high_resolution_clock::now();
    uint64_t nodecount = 0;
    int currentDepth = 1;
    bool stop = 0;
    vector<chess::Move> pv;
    chess::Move pvmove;

    auto checktime = [&]() -> bool {
        auto now = chrono::high_resolution_clock::now();
        int duration = chrono::duration_cast<chrono::milliseconds>(now - start).count();
        return (movetime - duration <= 30);
    };

    auto quiesce = [&](auto&& self, chess::Board& board, bool maximizingPlayer, float alpha, float beta) -> float {
        if ((++nodecount & nodespercheck) == 0) {
            if (checktime()) {
                stop = 1;
                return 0;
            }
        }
        float best_value;
        bool incheck = board.inCheck();
        chess::Movelist movelist;
        if (incheck) {
            chess::movegen::legalmoves(movelist, board);
            if (movelist.empty())
                return (board.sideToMove() ? INFINITY : -INFINITY);
        } else {
            chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(movelist, board);
        }

        if (!incheck) {
            best_value = eval(board);
            if (maximizingPlayer) {
                if (best_value >= beta) return best_value;
                alpha = max(alpha, best_value);
            } else {
                if (best_value <= alpha) return best_value;
                beta = min(beta, best_value);
            }
        } else {
            best_value = (maximizingPlayer ? -INFINITY : INFINITY);
        }

        move_valuing(movelist, board, chess::Move::NO_MOVE);

        if (maximizingPlayer) {
            for (int i = 0; i < movelist.size(); i++) {
                int best_index = i;
                for (int j = i+1; j < movelist.size(); j++) {
                    if (movelist[best_index].score() < movelist[j].score())
                        best_index = j;
                }
                swap(movelist[best_index], movelist[i]);
                chess::Move next_capture_move = movelist[i];
                board.makeMove(next_capture_move);
                float score = self(self, board, false, alpha, beta);
                board.unmakeMove(next_capture_move);
                if (stop) return 0;
                if (best_value < score) {
                    best_value = score;
                }
                alpha = max(alpha, score);
                if (alpha >= beta) break;
            }
        } else {
            for (int i = 0; i < movelist.size(); i++) {
                int best_index = i;
                for (int j = i+1; j < movelist.size(); j++) {
                    if (movelist[best_index].score() < movelist[j].score())
                        best_index = j;
                }
                swap(movelist[best_index], movelist[i]);
                chess::Move next_capture_move = movelist[i];
                board.makeMove(next_capture_move);
                float score = self(self, board, true, alpha, beta);
                board.unmakeMove(next_capture_move);
                if (stop) return 0;
                if (best_value > score) {
                    best_value = score;
                }
                beta = min(beta, score);
                if (alpha >= beta) break;
            }
        }
        return best_value;
    };

    auto minimax = [&](auto&& self, int depth, int ply, float alpha, float beta, bool maximizingPlayer) -> float {
        if ((++nodecount & nodespercheck) == 0) {
            if (checktime()) {
                stop = 1;
                return 0;
            }
        }
        if (board.isRepetition() || board.isInsufficientMaterial()) {
            return drawfactor;
        }
        if (board.isHalfMoveDraw()) {
            if (board.getHalfMoveDrawType().first == chess::GameResultReason::CHECKMATE)
                return (board.sideToMove() ? INFINITY : -INFINITY);
            else
                return drawfactor;
        }
        bool incheck = board.inCheck();
        TTEntry entry = TTTable::get(board);
        if (entry.key == board.hash() && entry.depth >= depth && !incheck) {
            if (entry.flag == EXACT) return entry.eval;
            if (entry.flag == LOWERBOUND && entry.eval >= beta) return entry.eval;
            if (entry.flag == UPPERBOUND && entry.eval <= alpha) return entry.eval;
        }

        float score;

        if (depth <= 0) {
            return quiesce(quiesce, board, maximizingPlayer, alpha, beta);
        }

        float static_eval = eval(board);

        bool nonpvnode = (beta - alpha == 1);

        if (!incheck && nonpvnode) {
            if (depth <= 3) {
                float margin = 100 * depth;
                if (maximizingPlayer) {
                    if (static_eval - margin >= beta)
                        return static_eval;
                } else {
                    if (static_eval + margin <= alpha) {
                        return static_eval;
                    }
                }
            }
            if (depth > R && board.hasNonPawnMaterial(board.sideToMove())) {
                board.makeNullMove();
                if (maximizingPlayer) {
                    score = self(self, depth - 1 - R, ply, beta - 1, beta, false);
                    board.unmakeNullMove();
                    if (score >= beta) return beta;
                }
                else {
                    score = self(self, depth - 1 - R, ply, alpha, alpha + 1, true);
                    board.unmakeNullMove();
                    if (score <= alpha) return alpha;
                }
            }
        }

        float original_alpha = alpha;
        float original_beta = beta;

        chess::Movelist movelist;
        chess::movegen::legalmoves(movelist, board);
        if (movelist.empty()) {
            if (incheck)
                return (board.sideToMove() ? INFINITY : -INFINITY);
            else
                return drawfactor;
        }

        chess::Move bestmoveply = movelist[0];

        int lmp_limit = 5 + 2 * depth * depth;

        int quietsearched = 0;

        if (ply == 0) {
            move_valuing(movelist, board, pvmove, depth, ply);
        } else {
            move_valuing(movelist, board, chess::Move::NO_MOVE, depth, ply);
        }

        //float fp_margin = depth * 150;

        if (maximizingPlayer) {
            //bool fp_prune = (static_eval + fp_margin <= alpha);
            float maxEval = -INFINITY;
            for (int i = 0; i < movelist.size(); i++) {
                int best_index = i;
                for (int j = i + 1; j < movelist.size(); j++) {
                    if (movelist[best_index].score() < movelist[j].score())
                        best_index = j;
                }
                swap(movelist[best_index], movelist[i]);
                chess::Move next_move = movelist[i];
                bool quietmove = silence_move(board, next_move);
                if (depth <= 2) {
                    if (quietmove && !incheck && nonpvnode) {
                        if (++quietsearched > lmp_limit) continue;
                    }
                }
                bool killermove = (next_move.score() == KillerMove1Score || next_move.score() == KillerMove2Score);
                board.makeMove(next_move);
                if (i == 0) {
                    score = self(self, depth - 1, ply + 1, alpha, beta, false);
                } else {
                    if (depth >= 3 && quietmove && !killermove && !incheck) {
                        int reduction = LMRTable[depth][i];
                        score = self(self, depth - 1 - reduction, ply + 1, alpha, alpha + 1, false);
                    } else {
                        score = self(self, depth - 1, ply + 1, alpha, alpha + 1, false);
                    }
                    if (score > alpha && score < beta) score = self(self, depth - 1, ply + 1, alpha, beta, false);
                }
                board.unmakeMove(next_move);
                if (stop) return 0;
                if (maxEval < score) {
                    maxEval = score;
                    bestmoveply = next_move;
                }
                alpha = max(alpha, score);
                if (alpha >= beta) {
                    TTTable::add(board, maxEval, bestmoveply, depth, LOWERBOUND);
                    if (quietmove) {
                        if (KillerMoves[ply][0] != next_move) {
                            KillerMoves[ply][1] = KillerMoves[ply][0];
                            KillerMoves[ply][0] = next_move;
                        }
                        if (ButterflyHeuristic[next_move.from().index()][next_move.to().index()] < HistoryLimit)
                            ButterflyHeuristic[next_move.from().index()][next_move.to().index()] += depth * depth;
                    }
                    return maxEval;
                }
            }

            if (maxEval <= original_alpha)
                TTTable::add(board, maxEval, bestmoveply, depth, UPPERBOUND);
            else
                TTTable::add(board, maxEval, bestmoveply, depth, EXACT);

            return maxEval;
        } else {
            //bool fp_prune = (static_eval - fp_margin >= beta);
            float minEval = INFINITY;
            for (int i = 0; i < movelist.size(); i++) {
                int best_index = i;
                for (int j = i + 1; j < movelist.size(); j++) {
                    if (movelist[best_index].score() < movelist[j].score())
                        best_index = j;
                }
                swap(movelist[best_index], movelist[i]);
                chess::Move next_move = movelist[i];
                bool quietmove = silence_move(board, next_move);
                if (depth <= 2) {
                    if (quietmove && !incheck && nonpvnode) {
                        if (++quietsearched > lmp_limit) continue;
                    }
                }
                bool killermove = (next_move.score() == KillerMove1Score || next_move.score() == KillerMove2Score);
                board.makeMove(next_move);
                if (i == 0) {
                    score = self(self, depth - 1, ply + 1, alpha, beta, true);
                } else {
                    if (depth >= 3 && quietmove && !killermove && !incheck) {
                        int reduction = LMRTable[depth][i];
                        score = self(self, depth - 1 - reduction, ply + 1, beta - 1, beta, true);
                    } else {
                        score = self(self, depth - 1, ply + 1, beta - 1, beta, true);
                    }
                    if (score < beta && score > alpha) score = self(self, depth - 1, ply + 1, alpha, beta, true);
                }
                board.unmakeMove(next_move);
                if (stop) return 0;
                if (minEval > score) {
                    minEval = score;
                    bestmoveply = next_move;
                }
                beta = min(beta, score);
                if (alpha >= beta) {
                    TTTable::add(board, minEval, bestmoveply, depth, UPPERBOUND);
                    if (quietmove) {
                        if (KillerMoves[ply][0] != next_move) {
                            KillerMoves[ply][1] = KillerMoves[ply][0];
                            KillerMoves[ply][0] = next_move;
                        }
                        if (ButterflyHeuristic[next_move.from().index()][next_move.to().index()] < HistoryLimit)
                            ButterflyHeuristic[next_move.from().index()][next_move.to().index()] += depth * depth;
                    }
                    return minEval;
                }
            }

            if (minEval >= original_beta)
                TTTable::add(board, minEval, bestmoveply, depth, LOWERBOUND);
            else
                TTTable::add(board, minEval, bestmoveply, depth, EXACT);

            return minEval;
        }
    };

    bool maximizingPlayer = !static_cast<bool>(board.sideToMove());
    chess::Move bestmove;
    float root_eval;

    float last_eval = 0;

    while (!stop) {
        string score, pvstring;
        float alpha = -INFINITY; float beta = INFINITY;
        bool fail = 1;
        int k = 0;
        while (fail) {
            k++;
            if (currentDepth > 1 && abs(last_eval) < INFINITY) {
                alpha = last_eval - k * delta;
                beta = last_eval + k * delta;
            }
            root_eval = minimax(minimax, currentDepth, 0, alpha, beta, maximizingPlayer);
            if (stop) break;
            if (root_eval == INFINITY || root_eval == -INFINITY) break;
            fail = !(root_eval > alpha && root_eval < beta);
        }
        if (stop) break;
        auto untilnow = chrono::high_resolution_clock::now();
        int time = chrono::duration_cast<chrono::milliseconds>(untilnow - start).count();
        last_eval = root_eval;
        TTEntry entry = TTTable::get(board);
        bestmove = entry.bestmove;
        if (root_eval == INFINITY) {
            if (board.sideToMove()) score = "mate " + to_string(-currentDepth);
            else score = "mate " + to_string(currentDepth);
            stop = 1;
        } else if (root_eval == -INFINITY) {
            if (board.sideToMove()) score = "mate " + to_string(currentDepth);
            else score = "mate " + to_string(-currentDepth);
            stop = 1;
        } else {
            score = "cp " + to_string((int)root_eval);
            if (root_eval == drawfactor) stop = 1;
        }

        for (int m = 0; m < currentDepth; m++) {
            chess::Move nextpv = TTTable::get_bestmove(board);
            if (nextpv == chess::Move::NO_MOVE) break;
            pvstring += chess::uci::moveToUci(nextpv) + " ";
            board.makeMove(nextpv);
            pv.push_back(nextpv);
        }

        pvmove = bestmove;

        int pvsize = pv.size();

        for (int n = 0; n < pvsize; n++) {
            board.unmakeMove(pv.back());
            pv.pop_back();
        }

        cout << "info depth " << currentDepth << " score " << score << " nodes " << nodecount << " time " << time << " pv " << pvstring << endl;

        if (currentDepth++ >= search_depth || time >= movetime) stop = 1;
    }
    cout << "bestmove " << chess::uci::moveToUci(bestmove) << endl;
}

int main() {
    cout.setf(std::ios::unitbuf);

    HistoryReset();
    init_tables();
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
            search(board, searchdepth, movetime);
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