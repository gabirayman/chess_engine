#include "chess/fen.hpp"
#include "chess/board.hpp"
#include "chess/defs.hpp"   // Color, Piece, BB(), CR_* etc.
#include <cctype>           // isdigit, isupper, tolower
#include <sstream>          // istringstream, ostringstream
#include <string>

namespace chess {

// Map a single FEN piece char onto the board at square index `sq` (0..63).
static bool putPiece(Board& b, int sq, char c) {
    const bool isWhite = std::isupper(static_cast<unsigned char>(c)) != 0;
    const Color col = isWhite ? WHITE : BLACK;
    const char k = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    Piece p;
    switch (k) {
        case 'p': p = PAWN;   break;
        case 'n': p = KNIGHT; break;
        case 'b': p = BISHOP; break;
        case 'r': p = ROOK;   break;
        case 'q': p = QUEEN;  break;
        case 'k': p = KING;   break;
        default: return false;
    }
    b.bb[col][p] |= BB(sq);
    return true;
}

bool setFromFEN(Board& b, std::string_view fen) {
    // Clear board bitboards
    for (int c = 0; c < COLOR_N; ++c)
        for (int p = 0; p < PIECE_N; ++p)
            b.bb[c][p] = 0ULL;

    std::istringstream ss{std::string(fen)};  // simple tokenizer
    std::string board, stm, cast, ep;
    int half = 0, full = 1;

    if (!(ss >> board >> stm >> cast >> ep >> half >> full)) {
        return false; // wrong number of fields
    }

    // 1) Piece placement: ranks 8..1
    int sq = 56;           // A8
    int filesInRank = 0;
    for (char ch : board) {
        if (ch == '/') {
            if (filesInRank != 8) return false;
            sq -= 16;              // down one rank (A8 -> A7 -> ... -> A1)
            filesInRank = 0;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            int run = ch - '0';
            if (run < 1 || run > 8 || filesInRank + run > 8) return false;
            sq += run;
            filesInRank += run;
        } else {
            if (!putPiece(b, sq, ch)) return false;
            ++sq;
            ++filesInRank;
        }
    }
    if (filesInRank != 8) return false;

    // 2) Side to move
    if (stm == "w")      b.sideToMove = WHITE;
    else if (stm == "b") b.sideToMove = BLACK;
    else return false;

    // 3) Castling rights
    b.castling = 0;
    if (cast != "-") {
        for (char c : cast) {
            if      (c == 'K') b.castling |= CR_WK;
            else if (c == 'Q') b.castling |= CR_WQ;
            else if (c == 'k') b.castling |= CR_BK;
            else if (c == 'q') b.castling |= CR_BQ;
            else return false;
        }
    }

    // 4) En passant target square (bitboard). FEN may give '-' or like "e3".
    b.epTarget = 0ULL;
    if (ep != "-") {
        if (ep.size() != 2) return false;
        char f = ep[0], r = ep[1];
        if (f < 'a' || f > 'h' || r < '1' || r > '8') return false;
        int file = f - 'a';
        int rank = r - '1';
        int epsq = rank * 8 + file;
        b.epTarget = BB(epsq);
    }

    // 5) Fifty-move halfmove clock, and 6) fullmove number
    if (half < 0 || full <= 0) return false;
    b.halfmoveClock = half;
    b.fullmoveNumber = full;

    // Recompute occupancies
    b.recompute();
    return true;
}

static inline char pieceChar(const Board& b, int sq) {
    const U64 m = BB(sq);
    if      (b.bb[WHITE][PAWN]   & m) return 'P';
    else if (b.bb[WHITE][KNIGHT] & m) return 'N';
    else if (b.bb[WHITE][BISHOP] & m) return 'B';
    else if (b.bb[WHITE][ROOK]   & m) return 'R';
    else if (b.bb[WHITE][QUEEN]  & m) return 'Q';
    else if (b.bb[WHITE][KING]   & m) return 'K';
    else if (b.bb[BLACK][PAWN]   & m) return 'p';
    else if (b.bb[BLACK][KNIGHT] & m) return 'n';
    else if (b.bb[BLACK][BISHOP] & m) return 'b';
    else if (b.bb[BLACK][ROOK]   & m) return 'r';
    else if (b.bb[BLACK][QUEEN]  & m) return 'q';
    else if (b.bb[BLACK][KING]   & m) return 'k';
    return 0;
}

std::string toFEN(const Board& b) {
    std::ostringstream out;

    // 1) Board
    for (int r = 7; r >= 0; --r) {
        int run = 0;
        for (int f = 0; f < 8; ++f) {
            int sq = r * 8 + f;
            char ch = pieceChar(b, sq);
            if (ch) {
                if (run) { out << run; run = 0; }
                out << ch;
            } else {
                ++run;
            }
        }
        if (run) out << run;
        if (r) out << '/';
    }

    // 2) Side to move
    out << ' ' << (b.sideToMove == WHITE ? 'w' : 'b') << ' ';

    // 3) Castling
    std::string cast;
    if (b.castling & CR_WK) cast += 'K';
    if (b.castling & CR_WQ) cast += 'Q';
    if (b.castling & CR_BK) cast += 'k';
    if (b.castling & CR_BQ) cast += 'q';
    out << (cast.empty() ? "-" : cast) << ' ';

    // 4) En passant
    if (b.epTarget == 0ULL) {
        out << '-';
    } else {
        int epsq = static_cast<int>(getSquare(b.epTarget));
        char file = static_cast<char>('a' + (epsq % 8));
        char rank = static_cast<char>('1' + (epsq / 8));
        out << file << rank;
    }

    // 5) Halfmove clock, 6) fullmove number
    out << ' ' << b.halfmoveClock << ' ' << b.fullmoveNumber;

    return out.str();
}

} // namespace chess
