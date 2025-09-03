#pragma once
#include <string>
#include <string_view>

namespace chess {

struct Board;  // fwd-decl

// Start position constant
inline constexpr std::string_view STARTPOS_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


bool setFromFEN(Board& b, std::string_view fen);

// convert a Board to FEN (always succeeds).
std::string toFEN(const Board& b);

} // namespace chess