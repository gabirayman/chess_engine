#pragma once
#include <cstdint>
#include <string>

#include "chess/defs.hpp"
#include "chess/move.hpp"

namespace chess {

struct Board;

// Perft stats
struct PerftStats {
    std::uint64_t nodes       = 0;
    std::uint64_t captures    = 0;
    std::uint64_t enPassant   = 0;
    std::uint64_t castles     = 0;
    std::uint64_t promotions  = 0;
    std::uint64_t checks      = 0;
    std::uint64_t checkmates  = 0;
};

// Count stats at an exact depth (depth >= 1).
// Convention: stats are counted on the lastply
//   depth=1: counts features of the root legal moves.
//   depth=2: counts features of moves one ply deeper, etc.
PerftStats perft_stats(const Board& b, int depth);

// text for a move like "e2e4", "e7e8q"
std::string toUci(const Move& m);

} // namespace chess
