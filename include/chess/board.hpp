#pragma once
#include <array>
#include "types.hpp"
#include "move.hpp"

namespace chess {

struct Board {
    // bitboards: bb[color][piece]
    std::array<std::array<U64, PIECE_N>, COLOR_N> bb{};

    U64 occ[COLOR_N] = {0, 0}; // occupancy for each color
    U64 occAll = 0; // occAll = occ[WHITE] | occ[BLACK]
    Color sideToMove {WHITE};

    uint8_t castling = CR_WK | CR_WQ | CR_BK | CR_BQ; // start: all rights available
    U64 epTarget = 0;     

    inline bool hasEP() const { return epTarget; }
    inline bool hasWK() const { return (castling & CR_WK); }
    inline bool hasWQ() const { return (castling & CR_WQ); }
    inline bool hasBK() const { return (castling & CR_BK); }
    inline bool hasBQ() const { return (castling & CR_BQ); }

    // recompute occupancies
    void recompute();

    void setStartPos();

    Piece pieceOn(int sq, Color c) const;
    Piece pieceOn(Square sq, Color c) const;

    static void printBB(U64 bitBoard);

    void printBoard() const;

    void applyMove(const Move& move);

    // void undoMove();
    
    bool canCastle() const;

    bool isSquareAttacked(Square sq, Color by) const;

    // bool isInCheck() const;

    // generate legal moves returns a vector of Move
    std::vector<Move> generateMoves() const;

    void genPawnMoves  (std::vector<Move>&, U64, int, Color, const Board&);
    void genKnightMoves(std::vector<Move>&, U64, int, Color, const Board&);
    void genBishopMoves(std::vector<Move>&, U64, int, Color, const Board&);
    void genRookMoves  (std::vector<Move>&, U64, int, Color, const Board&);
    void genQueenMoves (std::vector<Move>&, U64, int, Color, const Board&);
    void genKingMoves  (std::vector<Move>&, U64, int, Color, const Board&);

    // TODO figure out where to put function vector which holds these functions.


};



} // namespace chess
