#pragma once
#include <cstdint>

namespace chess
{
    using U16 = uint16_t;
    inline constexpr U16 MF_None       = 0;
    inline constexpr U16 MF_Capture    = 1u << 0;
    inline constexpr U16 MF_DoublePush = 1u << 1;
    inline constexpr U16 MF_EnPassant  = 1u << 2;
    inline constexpr U16 MF_CastleK    = 1u << 3;
    inline constexpr U16 MF_CastleQ    = 1u << 4;
    inline constexpr U16 MF_PromoQ     = 1u << 5;
    inline constexpr U16 MF_PromoR     = 1u << 6;
    inline constexpr U16 MF_PromoB     = 1u << 7;
    inline constexpr U16 MF_PromoN     = 1u << 8;

    struct Move {
        uint8_t from;
        uint8_t to;
        U16 flags;      // bitwise | of flags
        Piece piece;   // piece moving
        
        // if we want undo functionality, we can add:
        // Piece captured; // captured piece, or PIECE_N if none
    };

    inline constexpr bool has(U16 flags, U16 f) { return (flags & f) != 0; }

} // namespace chess