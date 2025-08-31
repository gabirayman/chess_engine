#include "chess/board.hpp"
#include <iostream>
#include <cassert>

namespace chess {

// recompute occ/occAll from bb[color][piece]
void Board::recompute() {
    occ[WHITE] = occ[BLACK] = 0ULL;
    for (int p = 0; p < PIECE_N; ++p) {
        occ[WHITE] |= bb[WHITE][p];
        occ[BLACK] |= bb[BLACK][p];
    }
    occAll = occ[WHITE] | occ[BLACK];
}

// load the standard chess starting position
void Board::setStartPos() {
    // clear everything
    for (int c = 0; c < COLOR_N; ++c)
        for (int p = 0; p < PIECE_N; ++p)
            bb[c][p] = 0ULL;

    // White
    bb[WHITE][PAWN]   = 0x000000000000FF00ULL;
    bb[WHITE][ROOK]   = 0x0000000000000081ULL;
    bb[WHITE][KNIGHT] = 0x0000000000000042ULL;
    bb[WHITE][BISHOP] = 0x0000000000000024ULL;
    bb[WHITE][QUEEN]  = 0x0000000000000008ULL;
    bb[WHITE][KING]   = 0x0000000000000010ULL;

    // Black
    bb[BLACK][PAWN]   = 0x00FF000000000000ULL;
    bb[BLACK][ROOK]   = 0x8100000000000000ULL;
    bb[BLACK][KNIGHT] = 0x4200000000000000ULL;
    bb[BLACK][BISHOP] = 0x2400000000000000ULL;
    bb[BLACK][QUEEN]  = 0x0800000000000000ULL;
    bb[BLACK][KING]   = 0x1000000000000000ULL;

    sideToMove = WHITE;
    recompute();
}

// Print a raw bitboard (bb) as 8 ranks of 8 zeros/ones (rank 8 to rank 1)
void Board::printBB(U64 bitBoard) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            U64 bit = 1ULL << sq;
            std::cout << ((bitBoard & bit) ? '1' : '0') << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}


void Board::printBoard() const {
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << ' ';
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            U64 m = BB(sq);

            char ch = '.';
            if      (bb[WHITE][PAWN]   & m) ch = 'P';
            else if (bb[WHITE][KNIGHT] & m) ch = 'N';
            else if (bb[WHITE][BISHOP] & m) ch = 'B';
            else if (bb[WHITE][ROOK]   & m) ch = 'R';
            else if (bb[WHITE][QUEEN]  & m) ch = 'Q';
            else if (bb[WHITE][KING]   & m) ch = 'K';
            else if (bb[BLACK][PAWN]   & m) ch = 'p';
            else if (bb[BLACK][KNIGHT] & m) ch = 'n';
            else if (bb[BLACK][BISHOP] & m) ch = 'b';
            else if (bb[BLACK][ROOK]   & m) ch = 'r';
            else if (bb[BLACK][QUEEN]  & m) ch = 'q';
            else if (bb[BLACK][KING]   & m) ch = 'k';

            std::cout << ch << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "  a b c d e f g h\n\n";
}

Piece Board::pieceOn(int sq, Color c) const {
    if (sq < 0 || sq > 63) {
            throw std::out_of_range("Square out of range");
        }
    U64 m = bb(sq);
    for (int p = 0; p < PIECE_N; ++p)
        if (bb[c][p] & m) return static_cast<Piece>(p);
    return PIECE_N;
}

Piece Board::pieceOn(Square sq, Color c) const {
    U64 m = BB(sq);
    for (int p = 0; p < PIECE_N; ++p)
        if (bb[c][p] & m) return static_cast<Piece>(p);
    return PIECE_N;
}

void Board::applyMove(const Move& move) {
    const Color us = sideToMove;
    const Color them = other(us);

    U64 fromBB = BB(move.from);
    U64 toBB = BB(move.to);

    assert((bb[us][move.piece] & fromBB) != 0); // piece must be on "from" square

    // remove piece from "from" square
    bb[us][move.piece] &= ~fromBB;

    // place piece on "to" square
    bb[us][move.piece] |= toBB; 

    // handle captures
    if (has(move.flags, MF_Capture)) {
        Piece captured = pieceOn(move.to, them); // find captured piece
        assert(captured != PIECE_N); // there must be a piece to capture
        bb[them][captured] &= ~toBB; // remove captured piece

    }

    // handle special moves
    // TODO: Add handling for promotions, castling, en passant, etc.

    // update occupancies
    recompute();
}

// vector<Move> Board::generateMoves() const {
//     std::vector<Move> moves;
//     for (int p = 0; p < PIECE_N; ++p) {
//         U64 pieces = bb[sideToMove][p];
//         while (pieces) {
//             int piece = pieces & -pieces; // get lowest set bit

//         }
//     }
// }

} // namespace chess