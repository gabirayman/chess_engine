#pragma once
#include <cstdint>
#include <stdexcept>

namespace chess
{
    using U64 = unsigned long long;

    enum Color : uint8_t { WHITE = 0, BLACK = 1, COLOR_N = 2 };
    enum Piece : uint8_t { PAWN = 0, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_N = 6 };

    // Returns the opposite color
    inline constexpr Color other(Color c) { return c == WHITE ? BLACK : WHITE; }

    enum Square : uint8_t {
        A1=0, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8 
    };
    
    // Bitboard with only the bit at square "sq" set, lsb = a1
    inline constexpr U64 BB(Square sq) { 
        return 1ULL << static_cast<unsigned>(sq); 
    }
    inline constexpr U64 BB(int sq) {
        if (sq < 0 || sq > 63) {
            throw std::out_of_range("Square out of range");
        }
        return 1ULL << static_cast<unsigned>(sq);
    }

    inline constexpr U64 FILE_A = 0x0101010101010101ULL;
    inline constexpr U64 FILE_H = 0x8080808080808080ULL;
    inline constexpr U64 RANK_1 = 0x00000000000000FFULL;
    inline constexpr U64 RANK_2 = 0x000000000000FF00ULL;
    inline constexpr U64 RANK_7 = 0x00FF000000000000ULL;
    inline constexpr U64 RANK_8 = 0xFF00000000000000ULL;

    enum CastlingRight : uint8_t {
        CR_WK = 1u << 0, // White O-O
        CR_WQ = 1u << 1, // White O-O-O
        CR_BK = 1u << 2, // Black O-O
        CR_BQ = 1u << 3  // Black O-O-O
    };


} // namespace chess