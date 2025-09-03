#include "chess/board.hpp"
#include "chess/fen.hpp"
#include "chess/debug.hpp"
#include "chess/move.hpp"
#include "chess/defs.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace chess;

static std::string sqToStr(int sq) {
    std::string s;
    s.push_back(static_cast<char>('a' + (sq % 8)));
    s.push_back(static_cast<char>('1' + (sq / 8)));
    return s;
}

static void printMove(const Move& m) {
    std::cout << sqToStr(m.from) << sqToStr(m.to);

    // flags
    if (has(m.flags, MF_CastleK)) std::cout << " (O-O)";
    else if (has(m.flags, MF_CastleQ)) std::cout << " (O-O-O)";
    else {
        if (has(m.flags, MF_Capture)) std::cout << " x";
        if (has(m.flags, MF_EnPassant)) std::cout << " e.p.";
        if (has(m.flags, MF_PromoQ)) std::cout << " =Q";
        else if (has(m.flags, MF_PromoR)) std::cout << " =R";
        else if (has(m.flags, MF_PromoB)) std::cout << " =B";
        else if (has(m.flags, MF_PromoN)) std::cout << " =N";
    }
}

int main(int argc, char** argv) {
    // Get FEN (argv or stdin; empty = startpos)
    std::string fen;
    if (argc > 1) {
        std::ostringstream oss;
        for (int i = 1; i < argc; ++i) {
            if (i > 1) oss << ' ';
            oss << argv[i];
        }
        fen = oss.str();
    } else {
        std::cout << "Enter FEN (empty = startpos):\n> ";
        std::getline(std::cin, fen);
    }
    if (fen.empty()) fen = std::string(STARTPOS_FEN);

    // Build board from FEN
    Board b;
    if (!setFromFEN(b, fen)) {
        std::cerr << "Invalid FEN.\n";
        return 1;
    }

    std::cout << "\nInitial position:\n";
    printBoard(b);

    const Color us   = b.sideToMove;
    const Color them = other(us);

    std::vector<Move> legal;
    auto pseudo = b.generateMoves();
    legal.reserve(pseudo.size());

    for (const Move& m : pseudo) {
        Board child = b.applied(m);

        int ksqAfter = (m.piece == KING)
            ? m.to
            : static_cast<int>(getSquare(child.bb[us][KING]));
        if (!child.isSquareAttacked(static_cast<Square>(ksqAfter), them)) {
            legal.push_back(m);
        }
    }

    std::cout << "Side to move: " << (us == WHITE ? "White" : "Black") << "\n";
    std::cout << "Legal moves: " << legal.size() << "\n\n";

    // Print all legal moves and resulting boards
    int idx = 1;
    for (const Move& m : legal) {
        std::cout << "#" << idx++ << " ";
        printMove(m);
        std::cout << "\n";

        Board child = b.applied(m);
        printBoard(child);
    }

    return 0;
}