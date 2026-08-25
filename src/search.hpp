#pragma once

#include <chrono>
#include <cmath>

#include "tt.hpp"
#include "eval.hpp"

using namespace std;

const int MAX_PLY = 128;
const int CONTEMPT_FACTOR = 15;
const int MATE_BASE = 100000;

const float delta = 25;
const int R = 3;
const float LMR_Scale = 3;
const float LMR_Base = 1;
const int LMP_DEPTH = 3;

const int FP_DEPTH = 4;

const int16_t TTMoveScore = 32767;
const int16_t PVMoveScore = 32766;
const int16_t CaptureBase = 32700;
const int16_t HistoryInit = -30000;
const int16_t HistoryLimit = 30000;
const int16_t KillerMove1Score = CaptureBase - 50;
const int16_t KillerMove2Score = CaptureBase - 100;

const int nodespercheck = (1 << 11) - 1;

const int16_t MVV_LVA[7][6] = {
    {15, 14, 13, 12, 11, 10}, // P
    {25, 24, 23, 22, 21, 20}, // N
    {35, 34, 33, 32, 31, 30}, // B
    {45, 44, 43, 42, 41, 40}, // R
    {55, 54, 53, 52, 51, 50}, // Q
    {0 , 0 , 0 , 0 , 0 , 0 }, // NONE
    {0 , 0 , 0 , 0 , 0 , 0 }, // NONE
//   P , N , B , R , Q , K
};

inline int16_t ButterflyHeuristic[64][64] = {0};

void HistoryReset() {
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            ButterflyHeuristic[y][x] = HistoryInit;
        }
    }
}

chess::Move KillerMoves[MAX_PLY][2];

int LMRTable[128][255] = {0};

void init_lmr() {
    for (int depth = 1; depth < 128; depth++) {
        for (int movecount = 1; movecount < 255; movecount++) {
            LMRTable[depth][movecount] = std::round(LMR_Base + log(depth) * log(movecount) / LMR_Scale);
        }
    }
}

inline bool silence_move(chess::Board& board, const chess::Move& move) {
    if (board.at(move.to()) != chess::Piece::NONE)
        return 0;
    if (move.typeOf() == chess::Move::PROMOTION || move.typeOf() == chess::Move::ENPASSANT)
        return 0;
    if (board.givesCheck(move) != chess::CheckType::NO_CHECK)
        return 0;
    return 1;
}

inline bool move_valuing(chess::Movelist& movelist, chess::Board& board, const chess::Move& pvmove, int depth = 0, int ply = MAX_PLY - 1) {
    bool ttMoveExists = 0;
    chess::Move ttmove = TTTable::get_bestmove(board, depth);
    for (int i = 0; i < movelist.size(); i++) {
        chess::Move& move = movelist[i];
        if (move == ttmove) {
            ttMoveExists = 1;
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
    return ttMoveExists;
}

struct PVTable {
    inline static void copy(const chess::Move& bestmove, int ply) {
        data[ply][0] = bestmove;
        size_t child_pv_size = size[ply + 1];
        memcpy(&data[ply][1], &data[ply + 1][0], child_pv_size * 4);
        size[ply] = child_pv_size + 1;
    }
    inline static char print() {
        size_t pvsize = size[0];
        for (int i = 0; i < pvsize; i++) {
            cout << chess::uci::moveToUci(data[0][i]) << " ";
        }
        return '\n';
    }
    inline static chess::Move data[MAX_PLY][MAX_PLY];
    inline static uint8_t size[MAX_PLY];
};

class Search {
public:
    Search(chess::Board& brd, int search_depth, int movetime) : movetime(movetime), board(brd) {
        rootMaximizing = !static_cast<bool>(board.sideToMove());
        float root_eval, last_eval;
        chess::Move bestmove;
        while (!stop) {
            float alpha = -INFINITY; float beta = INFINITY;
            bool fail = 1;
            int a{1}, b{1};
            seldepth = 0;
            while (fail) {
                if (currentDepth > 1 && abs(last_eval) <= MATE_BASE - MAX_PLY) {
                    alpha = last_eval - a * delta;
                    beta = last_eval + b * delta;
                }
                root_eval = minimax(currentDepth, 0, alpha, beta, rootMaximizing);
                if (stop) break;
                if (abs(root_eval) > MATE_BASE - MAX_PLY) break;
                if (root_eval < alpha) {
                    scoreuci = "cp " + to_string((int)root_eval) + " upperbound";
                    log_uci_info_string();
                    a += 3;
                } else if (root_eval > beta) {
                    scoreuci = "cp " + to_string((int)root_eval) + " lowerbound";
                    log_uci_info_string();
                    b += 3;
                } else {
                    fail = 0;
                }
            }
            if (stop) break;
            last_eval = root_eval;
            bestmove = PVTable::data[0][0];
            if (abs(root_eval) > MATE_BASE - MAX_PLY) {
                bool side2Mate = !(root_eval > 0);
                int distanceToMate = (side2Mate == board.sideToMove() ? MATE_BASE - abs(root_eval) : abs(root_eval) - MATE_BASE);
                scoreuci = "mate " + to_string(distanceToMate);
                stop = 1;
            } else {
                scoreuci = "cp " + to_string((int)root_eval);
            }

            pvmove = bestmove;

            log_uci_info_string();

            if (currentDepth++ >= search_depth) stop = 1;
        }
        cout << "bestmove " << chess::uci::moveToUci(bestmove) << endl;
    }

    bool checktime() {
        auto now = chrono::high_resolution_clock::now();
        int duration = chrono::duration_cast<chrono::milliseconds>(now - start).count();
        return (movetime - duration <= 30);
    }

    float quiesce(int qply, float alpha, float beta, bool maximizingPlayer) {
        if ((nodecount & nodespercheck) == 0) {
            if (checktime()) {
                stop = 1;
                return 0;
            }
        }
        int ply = qply + leafnode_ply;
        seldepth = max(seldepth, ply);
        float best_value;
        bool incheck = board.inCheck();
        chess::Movelist movelist;
        if (incheck) {
            chess::movegen::legalmoves(movelist, board);
            if (movelist.empty()) {
                return (board.sideToMove() ? (MATE_BASE - ply) : -(MATE_BASE - ply));
            }
        } else {
            chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(movelist, board);
        }

        if (!incheck) {
            best_value = Eval::eval(board);
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
                nodecount++;
                float score = quiesce(qply + 1, alpha, beta, false);
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
                nodecount++;
                float score = quiesce(qply + 1, alpha, beta, true);
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
    }

    float minimax(int depth, int ply, float alpha, float beta, bool maximizingPlayer) {
        if ((nodecount & nodespercheck) == 0) {
            if (checktime()) {
                stop = 1;
                return 0;
            }
        }
        PVTable::size[ply] = 0;
        if (board.isRepetition() || board.isInsufficientMaterial()) {
            return (board.sideToMove() ? -CONTEMPT_FACTOR : CONTEMPT_FACTOR);
        }
        if (board.isHalfMoveDraw()) {
            if (board.getHalfMoveDrawType().first == chess::GameResultReason::CHECKMATE)
                return (board.sideToMove() ? (MATE_BASE - ply) : -(MATE_BASE - ply));
            else
                return (board.sideToMove() ? -CONTEMPT_FACTOR : CONTEMPT_FACTOR);
        }
        bool incheck = board.inCheck();
        TTEntry entry = TTTable::get(board);
        if (ply > 0 && entry.key == board.hash() && entry.depth >= depth) {
            if (entry.flag == EXACT) return entry.eval;
            if (entry.flag == LOWERBOUND && entry.eval >= beta) return entry.eval;
            if (entry.flag == UPPERBOUND && entry.eval <= alpha) return entry.eval;
        }

        if (depth <= 0) {
            leafnode_ply = ply;
            return quiesce(0, alpha, beta, maximizingPlayer);
        }

        float score;

        float static_eval = Eval::eval(board);

        bool nonpvnode = (beta - alpha == 1);

        bool basic_pruning_condition = !incheck && nonpvnode;

        bool allow_futility_pruning = 0;

        if (basic_pruning_condition) {
            int rfp_margin = 150 * depth;
            int fp_margin = 85 * depth + 45;
            if (maximizingPlayer) {
                allow_futility_pruning = depth <= FP_DEPTH && static_eval + fp_margin <= alpha;
                if (static_eval - rfp_margin >= beta)
                    return static_eval;
            } else {
                allow_futility_pruning = depth <= FP_DEPTH && static_eval - fp_margin >= beta;
                if (static_eval + rfp_margin <= alpha) {
                    return static_eval;
                }
            }
            if (depth >= R && board.hasNonPawnMaterial(board.sideToMove())) {
                board.makeNullMove();
                if (maximizingPlayer) {
                    score = minimax(depth - R, ply + 1, alpha, beta, false);
                    board.unmakeNullMove();
                    if (score >= beta) return beta;
                }
                else {
                    score = minimax(depth - R, ply + 1, alpha, beta, true);
                    board.unmakeNullMove();
                    if (score <= alpha) return alpha;
                }
            }
        }

        chess::Movelist& movelist = movelists[ply];
        chess::movegen::legalmoves(movelist, board);
        if (movelist.empty()) {
            if (incheck)
                return (board.sideToMove() ? (MATE_BASE - ply) : -(MATE_BASE - ply));
            else
                return (board.sideToMove() ? -CONTEMPT_FACTOR : CONTEMPT_FACTOR);
        }

        chess::Move bestmove = movelist[0];

        int lmp_limit = 4 + depth * depth;

        int quietsearched = 0;

        bool ttMoveExists;

        if (ply == 0) {
            ttMoveExists = move_valuing(movelist, board, pvmove, depth, ply);
        } else {
            ttMoveExists = move_valuing(movelist, board, chess::Move::NO_MOVE, depth, ply);
        }

        if (depth > 9 && !ttMoveExists) depth--;

        //NOTE FOR ME: THERE'S A PROBLEM IN HISTORY HEURISTIC, WHERE MOVES ARE REGARDED AS QUIET IF ITS SCORE IS LOWER THAN CAPTURE BASE
        //HOWEVER, ON CHESSPROGRAMMINGWIKI, WRITING A MOVE TO HISTORY IS NOT NECESSARY 'QUIET', IS NON CAPTURE MOVE

        if (maximizingPlayer) {
            float original_alpha = alpha;
            float maxEval = -INFINITY;
            for (int i = 0; i < movelist.size(); i++) {
                int best_index = i;
                for (int j = i + 1; j < movelist.size(); j++) {
                    if (movelist[best_index].score() < movelist[j].score())
                        best_index = j;
                }
                swap(movelist[best_index], movelist[i]);
                chess::Move next_move = movelist[i];
                bool quietmove = next_move.score() < CaptureBase;
                if (quietmove) {
                    if (allow_futility_pruning && maxEval > -INFINITY) break;
                    quietsearched++;
                }
                if (depth <= LMP_DEPTH && basic_pruning_condition) {
                    if (quietsearched > lmp_limit) break;
                }
                //bool killermove = (next_move.score() == KillerMove1Score || next_move.score() == KillerMove2Score);
                board.makeMove(next_move);
                nodecount++;
                if (i == 0) {
                    score = minimax(depth - 1, ply + 1, alpha, beta, false);
                } else {
                    if (depth >= 3 && quietmove && !incheck) {
                        int reduction = LMRTable[depth][i];
                        score = minimax(depth - reduction, ply + 1, alpha, alpha + 1, false);
                    } else {
                        score = minimax(depth - 1, ply + 1, alpha, alpha + 1, false);
                    }
                    if (score > alpha && score < beta) score = minimax(depth - 1, ply + 1, alpha, beta, false);
                }
                board.unmakeMove(next_move);
                if (stop) return 0;
                if (maxEval < score) {
                    maxEval = score;
                    bestmove = next_move;
                    PVTable::copy(bestmove, ply);
                }
                alpha = max(alpha, score);
                if (alpha >= beta) {
                    TTTable::add(board, maxEval, bestmove, depth, LOWERBOUND);
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
                TTTable::add(board, maxEval, bestmove, depth, UPPERBOUND);
            else
                TTTable::add(board, maxEval, bestmove, depth, EXACT);

            return maxEval;
        } else {
            float original_beta = beta;
            float minEval = INFINITY;
            for (int i = 0; i < movelist.size(); i++) {
                int best_index = i;
                for (int j = i + 1; j < movelist.size(); j++) {
                    if (movelist[best_index].score() < movelist[j].score())
                        best_index = j;
                }
                swap(movelist[best_index], movelist[i]);
                chess::Move next_move = movelist[i];
                bool quietmove = next_move.score() < CaptureBase;
                if (quietmove) {
                    if (allow_futility_pruning && minEval < INFINITY) break;
                    quietsearched++;
                }
                if (depth <= LMP_DEPTH && basic_pruning_condition) {
                    if (quietsearched > lmp_limit) break;
                }
                //bool killermove = (next_move.score() == KillerMove1Score || next_move.score() == KillerMove2Score);
                board.makeMove(next_move);
                nodecount++;
                if (i == 0) {
                    score = minimax(depth - 1, ply + 1, alpha, beta, true);
                } else {
                    if (depth >= 3 && quietmove && !incheck) {
                        int reduction = LMRTable[depth][i];
                        score = minimax(depth - reduction, ply + 1, beta - 1, beta, true);
                    } else {
                        score = minimax(depth - 1, ply + 1, beta - 1, beta, true);
                    }
                    if (score < beta && score > alpha) score = minimax(depth - 1, ply + 1, alpha, beta, true);
                }
                board.unmakeMove(next_move);
                if (stop) return 0;
                if (minEval > score) {
                    minEval = score;
                    bestmove = next_move;
                    PVTable::copy(bestmove, ply);
                }
                beta = min(beta, score);
                if (alpha >= beta) {
                    TTTable::add(board, minEval, bestmove, depth, UPPERBOUND);
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
                TTTable::add(board, minEval, bestmove, depth, LOWERBOUND);
            else
                TTTable::add(board, minEval, bestmove, depth, EXACT);

            return minEval;
        }
    }

private:

    void log_uci_info_string() {
        auto untilnow = chrono::high_resolution_clock::now();
        int time = chrono::duration_cast<chrono::microseconds>(untilnow - start).count();
        float time_milli = time / 1000.0f;
        int nps = nodecount / (time_milli / 1000.0f);
        cout << "info depth " << currentDepth << " seldepth " << seldepth << " score " << scoreuci << " nodes " << nodecount << " time " << int(time_milli) << " nps " << nps << " hashfull " << TTTable::hashfull() << " pv " << PVTable::print();
        if (movetime - time_milli <= 30) stop = 1;
    }

    const int movetime;
    chrono::time_point<chrono::system_clock, chrono::system_clock::duration> start = chrono::high_resolution_clock::now();
    chess::Movelist movelists[MAX_PLY];
    chess::Board& board;
    uint64_t nodecount = 0;
    bool stop = 0;
    int currentDepth = 1;
    int seldepth = 0;
    int leafnode_ply;
    chess::Move pvmove;
    bool rootMaximizing;
    string scoreuci;
};