#include "chess/perft.hpp"
#include "chess/board.hpp"

#include <string>

namespace chess {

static char promoChar(const Move& m) {
    if (has(m.flags, MF_PromoQ)) return 'q';
    if (has(m.flags, MF_PromoR)) return 'r';
    if (has(m.flags, MF_PromoB)) return 'b';
    if (has(m.flags, MF_PromoN)) return 'n';
    return '\0';
}

static std::string sqToStr(int sq) {
    std::string s;
    s.push_back(static_cast<char>('a' + (sq % 8)));
    s.push_back(static_cast<char>('1' + (sq / 8)));
    return s;
}

std::string toUci(const Move& m) {
    std::string s = sqToStr(m.from) + sqToStr(m.to);
    if (char p = promoChar(m)) s.push_back(p);
    return s;
}

// recursive helper that accumulates only at the last ply
static void rec_stats(const Board& b, int depth, PerftStats& out) {
    auto moves = b.generateLegalMoves();

    if (depth == 1) {
        // Count features of these leaf moves
        for (const auto& mv : moves) {
            out.nodes += 1;

            if (has(mv.flags, MF_Capture)) {
                out.captures += 1;
            }
            if (has(mv.flags, MF_EnPassant)) {
                out.enPassant += 1;
            }
            if (has(mv.flags, MF_CastleK) || has(mv.flags, MF_CastleQ)) {
                out.castles += 1;
            }                   
            if (has(mv.flags, MF_PromoQ) || has(mv.flags, MF_PromoR) ||
                has(mv.flags, MF_PromoB) || has(mv.flags, MF_PromoN)) {
                    out.promotions += 1;
            }
                                               

            Board child = b.applied(mv);
            // After move, opponent is to move. If they are in check -> this move gives check.
            if (child.isInCheck(child.sideToMove)) out.checks += 1;
            if (child.isCheckmate())               out.checkmates += 1;
        }
        return;
    }

    // Otherwise go deeper
    for (const auto& mv : moves) {
        Board child = b.applied(mv);
        rec_stats(child, depth - 1, out);
    }
}

PerftStats perft_stats(const Board& b, int depth) {
    PerftStats s{};
    if (depth <= 0) return s;
    rec_stats(b, depth, s);
    return s;
}

} // namespace chess
