#pragma once
#include <array>
#include <vector> 
#include "defs.hpp"
#include "move.hpp"

namespace chess {

struct Board {
    // bitboards: bb[color][piece]
    std::array<std::array<U64, PIECE_N>, COLOR_N> bb{};

    U64 occ[COLOR_N] = {0, 0}; // occupancy for each color
    U64 occAll = 0; // occAll = occ[WHITE] | occ[BLACK]
    Color sideToMove {WHITE};

    uint8_t castling = CR_WK | CR_WQ | CR_BK | CR_BQ; // start: all rights available
    int  halfmoveClock = 0;
    int  fullmoveNumber = 1;
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

    void applyMove(const Move& move);

    Board applied(const Move& move) const;

    // void undoMove();
    
    bool canCastleKingSide(Color c) const;

    bool canCastleQueenSide(Color c) const;

    bool isSquareAttacked(Square sq, Color by) const;

    bool isInCheck(Color c) const;
    bool isInCheck() const; // uses sideToMove

    bool hasLegalMove() const;
    bool isCheckmate() const;
    bool isStalemate() const;
    GameState state() const;

    static void push_promos(std::vector<Move>& moves, int from, int to, U16 baseFlags);

    // generate legal moves returns a vector of Move
    std::vector<Move> generateMoves() const;
    std::vector<Move> generateLegalMoves() const;

    static void genPawnMoves  (std::vector<Move>&, U64, int, Color, const Board&);
    static void genKnightMoves(std::vector<Move>&, U64, int, Color, const Board&);
    static void genBishopMoves(std::vector<Move>&, U64, int, Color, const Board&);
    static void genRookMoves  (std::vector<Move>&, U64, int, Color, const Board&);
    static void genQueenMoves (std::vector<Move>&, U64, int, Color, const Board&);
    static void genKingMoves  (std::vector<Move>&, U64, int, Color, const Board&);

    static void genDiagonalMoves(std::vector<Move>&, U64, int, Color, const Board&, Piece piece);
    static void genStraightMoves(std::vector<Move>&, U64, int, Color, const Board&, Piece piece);


};

// array of pointers that exists outside of the class so it will not be re-created for each instance of Board

using MoveGenFn = void (*)(std::vector<Move>&, U64, int, Color, const Board&);

inline constexpr std::array<MoveGenFn, PIECE_N> pieceFunctionArray = {
    &Board::genPawnMoves,
    &Board::genKnightMoves,
    &Board::genBishopMoves,
    &Board::genRookMoves,
    &Board::genQueenMoves,
    &Board::genKingMoves
};


} // namespace chess
