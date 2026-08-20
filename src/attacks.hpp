#pragma once

#include <functional>
#include <utility>

#include "Pext.hpp"
#include "attacks_fwd.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "coords.hpp"
#include "piece.hpp"

namespace chess {

template <Direction direction>
[[nodiscard]] inline constexpr Bitboard attacks::shift(const Bitboard b) {
    switch (direction) {
        case Direction::NORTH:
            return b << 8;
        case Direction::SOUTH:
            return b >> 8;
        case Direction::NORTH_WEST:
            return (b & ~MASK_FILE[0]) << 7;
        case Direction::WEST:
            return (b & ~MASK_FILE[0]) >> 1;
        case Direction::SOUTH_WEST:
            return (b & ~MASK_FILE[0]) >> 9;
        case Direction::NORTH_EAST:
            return (b & ~MASK_FILE[7]) << 9;
        case Direction::EAST:
            return (b & ~MASK_FILE[7]) << 1;
        case Direction::SOUTH_EAST:
            return (b & ~MASK_FILE[7]) >> 7;
    }

    assert(false);

    return {};
}

template <Color::underlying c>
[[nodiscard]] inline Bitboard attacks::pawnLeftAttacks(const Bitboard pawns) {
    return c == Color::WHITE ? (pawns << 7) & ~MASK_FILE[static_cast<int>(File::FILE_H)]
                             : (pawns >> 7) & ~MASK_FILE[static_cast<int>(File::FILE_A)];
}

template <Color::underlying c>
[[nodiscard]] inline Bitboard attacks::pawnRightAttacks(const Bitboard pawns) {
    return c == Color::WHITE ? (pawns << 9) & ~MASK_FILE[static_cast<int>(File::FILE_A)]
                             : (pawns >> 9) & ~MASK_FILE[static_cast<int>(File::FILE_H)];
}

[[nodiscard]] inline Bitboard attacks::pawn(Color c, Square sq) noexcept { return PawnAttacks[c][sq.index()]; }

[[nodiscard]] inline Bitboard attacks::knight(Square sq) noexcept { return KnightAttacks[sq.index()]; }

[[nodiscard]] inline Bitboard attacks::bishop(Square sq, Bitboard occupied) noexcept {
    return Chess_Lookup::Lookup_Pext::Bishop(sq.index(), occupied.getBits());
}

[[nodiscard]] inline Bitboard attacks::rook(Square sq, Bitboard occupied) noexcept {
    return Chess_Lookup::Lookup_Pext::Rook(sq.index(), occupied.getBits());
}

[[nodiscard]] inline Bitboard attacks::queen(Square sq, Bitboard occupied) noexcept {
    return Chess_Lookup::Lookup_Pext::Queen(sq.index(), occupied.getBits());
}

[[nodiscard]] inline Bitboard attacks::king(Square sq) noexcept { return KingAttacks[sq.index()]; }

[[nodiscard]] inline Bitboard attacks::attackers(const Board& board, Color color, Square square) noexcept {
    const auto queens   = board.pieces(PieceType::QUEEN, color);
    const auto occupied = board.occ();

    // using the fact that if we can attack PieceType from square, they can attack us back
    auto atks = (pawn(~color, square) & board.pieces(PieceType::PAWN, color));
    atks |= (knight(square) & board.pieces(PieceType::KNIGHT, color));
    atks |= (bishop(square, occupied) & (board.pieces(PieceType::BISHOP, color) | queens));
    atks |= (rook(square, occupied) & (board.pieces(PieceType::ROOK, color) | queens));
    atks |= (king(square) & board.pieces(PieceType::KING, color));

    return atks & occupied;
}

template <PieceType::underlying pt>
[[nodiscard]] inline Bitboard attacks::slider(Square sq, Bitboard occupied) noexcept {
    static_assert(pt == PieceType::BISHOP || pt == PieceType::ROOK || pt == PieceType::QUEEN,
                  "PieceType must be a slider!");

    if constexpr (pt == PieceType::BISHOP) return bishop(sq, occupied);
    if constexpr (pt == PieceType::ROOK) return rook(sq, occupied);
    if constexpr (pt == PieceType::QUEEN) return queen(sq, occupied);
}

template <bool ISROOK>
[[nodiscard]] inline Bitboard attacks::sliderAttacks(Square sq, Bitboard occupied) noexcept {
    static constexpr int dirs[2][4][2] = {{{1, 1}, {1, -1}, {-1, -1}, {-1, 1}}, {{1, 0}, {0, -1}, {-1, 0}, {0, 1}}};

    Bitboard attacks = 0ull;

    File pf = sq.file();
    Rank pr = sq.rank();

    for (int i = 0; i < 4; ++i) {
        int off_f = dirs[ISROOK][i][0];
        int off_r = dirs[ISROOK][i][1];

        File f;
        Rank r;
        for (f = pf + off_f, r = pr + off_r; Square::is_valid(r, f); f += off_f, r += off_r) {
            const auto index = Square(f, r).index();
            attacks.set(index);
            if (occupied.check(index)) break;
        }
    }

    return attacks;
}

}  // namespace chess
