#pragma once
#include <cstdint>
#include <bit>
#include <array>
#include <cassert>

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
    inline U64 BB(int sq) {
        assert(0 <= sq && sq < 64);
        return 1ULL << static_cast<unsigned>(sq);
    }

    inline constexpr Square getSquare(U64 bitboard) {
        return static_cast<Square>(std::countr_zero(bitboard));
    }

    inline constexpr U64 FILE_A = 0x0101010101010101ULL;
    inline constexpr U64 FILE_B = 0x0202020202020202ULL;
    inline constexpr U64 FILE_G = 0x4040404040404040ULL;
    inline constexpr U64 FILE_H = 0x8080808080808080ULL;
    inline constexpr U64 RANK_1 = 0x00000000000000FFULL;
    inline constexpr U64 RANK_2 = 0x000000000000FF00ULL;
    inline constexpr U64 RANK_7 = 0x00FF000000000000ULL;
    inline constexpr U64 RANK_8 = 0xFF00000000000000ULL;

    inline constexpr U64 FILE_AB = FILE_A | FILE_B;
    inline constexpr U64 FILE_GH = FILE_G | FILE_H;

    enum CastlingRight : uint8_t { // to keep track if king and roook have moved
        CR_WK = 1u << 0, // White king side
        CR_WQ = 1u << 1, // White queen side
        CR_BK = 1u << 2, // Black king side
        CR_BQ = 1u << 3  // Black queen side
    };

    //helper function to generate knight attack bitboards
    constexpr U64 knight_from(U64 from) { 
        U64 a = 0;
        a |= (from & ~FILE_H) << 17;
        a |= (from & ~FILE_A) << 15;
        a |= (from & ~FILE_GH) << 10;
        a |= (from & ~FILE_AB) <<  6;
        a |= (from & ~FILE_H) >> 15;
        a |= (from & ~FILE_A) >> 17;
        a |= (from & ~FILE_GH) >>  6;
        a |= (from & ~FILE_AB) >> 10;
        return a;
    }

    // Precomputed knight attack targets for each square
    inline constexpr std::array<U64, 64> KNIGHT_ATTACK_TARGETS = []{
        std::array<U64, 64> t{};
        for (int sq = 0; sq < 64; ++sq) t[sq] = knight_from(1ULL << sq);
        return t;
    }();

    //helper function to generate king attack bitboards
    constexpr U64 king_from(U64 from) { 
        U64 a = 0;
        a |= (from) << 8;
        a |= (from) >> 8;
        a |= (from & ~FILE_H) << 9;
        a |= (from & ~FILE_A) >> 9;
        a |= (from & ~FILE_A)  << 7;
        a |= (from & ~FILE_H)  >> 7;
        a |= (from & ~FILE_H) << 1;
        a |= (from & ~FILE_A) >> 1;
        return a;
    }

    // Precomputed king attack targets for each square
    inline constexpr std::array<U64, 64> KING_ATTACK_TARGETS = []{
        std::array<U64, 64> t{};
        for (int sq = 0; sq < 64; ++sq) t[sq] = king_from(1ULL << sq);
        return t;
    }();


} // namespace chess