#include "chess/debug.hpp"
#include "chess/board.hpp"
#include "chess/defs.hpp"   
#include <ostream>          
#include <iostream> // std::cout

namespace chess {

void printBB(U64 bitboard, std::ostream& os) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            os << (((bitboard >> sq) & 1ULL) ? '1' : '0') << ' ';
        }
        os << '\n';
    }
    os << '\n';
}

void printBB(U64 bitboard) {
    printBB(bitboard, std::cout);
}

void printBoard(const Board& b, std::ostream& os) {
    for (int rank = 7; rank >= 0; --rank) {
        os << (rank + 1) << ' ';
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            U64 m = BB(sq);

            char ch = '.';
            if      (b.bb[WHITE][PAWN]   & m) ch = 'P';
            else if (b.bb[WHITE][KNIGHT] & m) ch = 'N';
            else if (b.bb[WHITE][BISHOP] & m) ch = 'B';
            else if (b.bb[WHITE][ROOK]   & m) ch = 'R';
            else if (b.bb[WHITE][QUEEN]  & m) ch = 'Q';
            else if (b.bb[WHITE][KING]   & m) ch = 'K';
            else if (b.bb[BLACK][PAWN]   & m) ch = 'p';
            else if (b.bb[BLACK][KNIGHT] & m) ch = 'n';
            else if (b.bb[BLACK][BISHOP] & m) ch = 'b';
            else if (b.bb[BLACK][ROOK]   & m) ch = 'r';
            else if (b.bb[BLACK][QUEEN]  & m) ch = 'q';
            else if (b.bb[BLACK][KING]   & m) ch = 'k';

            os << ch << ' ';
        }
        os << '\n';
    }
    os << "  a b c d e f g h\n\n";
}

void printBoard(const Board& b) {
    printBoard(b, std::cout);
}

} // namespace chess