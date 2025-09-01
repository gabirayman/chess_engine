#include "chess/board.hpp"
#include "chess/defs.hpp"
#include <iostream>
#include <cassert>

namespace chess {

// recompute occ/occAll from bb[color][piece]
void Board::recompute() {
    occ[WHITE] = occ[BLACK] = 0ULL;
    for (int p = 0; p < PIECE_N; ++p) {
        occ[WHITE] |= bb[WHITE][p];
        occ[BLACK] |= bb[BLACK][p];
    }
    occAll = occ[WHITE] | occ[BLACK];
}

// load the standard chess starting position
void Board::setStartPos() {
    // clear everything
    for (int c = 0; c < COLOR_N; ++c)
        for (int p = 0; p < PIECE_N; ++p)
            bb[c][p] = 0ULL;

    // White
    bb[WHITE][PAWN]   = 0x000000000000FF00ULL;
    bb[WHITE][ROOK]   = 0x0000000000000081ULL;
    bb[WHITE][KNIGHT] = 0x0000000000000042ULL;
    bb[WHITE][BISHOP] = 0x0000000000000024ULL;
    bb[WHITE][QUEEN]  = 0x0000000000000008ULL;
    bb[WHITE][KING]   = 0x0000000000000010ULL;

    // Black
    bb[BLACK][PAWN]   = 0x00FF000000000000ULL;
    bb[BLACK][ROOK]   = 0x8100000000000000ULL;
    bb[BLACK][KNIGHT] = 0x4200000000000000ULL;
    bb[BLACK][BISHOP] = 0x2400000000000000ULL;
    bb[BLACK][QUEEN]  = 0x0800000000000000ULL;
    bb[BLACK][KING]   = 0x1000000000000000ULL;

    sideToMove = WHITE;
    recompute();
}

// Print a raw bitboard (bb) as 8 ranks of 8 zeros/ones (rank 8 to rank 1)
void Board::printBB(U64 bitBoard) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            U64 bit = 1ULL << sq;
            std::cout << ((bitBoard & bit) ? '1' : '0') << ' ';
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}


void Board::printBoard() const {
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << ' ';
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            U64 m = BB(sq);

            char ch = '.';
            if      (bb[WHITE][PAWN]   & m) ch = 'P';
            else if (bb[WHITE][KNIGHT] & m) ch = 'N';
            else if (bb[WHITE][BISHOP] & m) ch = 'B';
            else if (bb[WHITE][ROOK]   & m) ch = 'R';
            else if (bb[WHITE][QUEEN]  & m) ch = 'Q';
            else if (bb[WHITE][KING]   & m) ch = 'K';
            else if (bb[BLACK][PAWN]   & m) ch = 'p';
            else if (bb[BLACK][KNIGHT] & m) ch = 'n';
            else if (bb[BLACK][BISHOP] & m) ch = 'b';
            else if (bb[BLACK][ROOK]   & m) ch = 'r';
            else if (bb[BLACK][QUEEN]  & m) ch = 'q';
            else if (bb[BLACK][KING]   & m) ch = 'k';

            std::cout << ch << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "  a b c d e f g h\n\n";
}

Piece Board::pieceOn(int sq, Color c) const {
    assert(0 <= sq && sq < 64);
    U64 m = BB(sq);
    for (int p = 0; p < PIECE_N; ++p)
        if (bb[c][p] & m) return static_cast<Piece>(p);
    return PIECE_N;
}

Piece Board::pieceOn(Square sq, Color c) const {
    U64 m = BB(sq);
    for (int p = 0; p < PIECE_N; ++p)
        if (bb[c][p] & m) return static_cast<Piece>(p);
    return PIECE_N;
}

void Board::applyMove(const Move& move) {
    const Color us = sideToMove;
    const Color them = other(us);

    U64 fromBB = BB(move.from);
    U64 toBB = BB(move.to);

    assert((bb[us][move.piece] & fromBB) != 0); // piece must be on "from" square

    // remove piece from "from" square
    bb[us][move.piece] &= ~fromBB;

    // place piece on "to" square
    bb[us][move.piece] |= toBB; 

    // handle captures
    if (has(move.flags, MF_Capture)) {
        Piece captured = pieceOn(move.to, them); // find captured piece
        assert(captured != PIECE_N); // there must be a piece to capture
        bb[them][captured] &= ~toBB; // remove captured piece

    }

    // handle special moves
    // TODO: Add handling for promotions, castling, en passant, etc.

    //handle flags

    // update occupancies
    recompute();
    sideToMove = them; // change turn
}

Board Board::applied(const Move& move) const {
    Board newBoard = *this; // copy current board
    newBoard.applyMove(move); // apply move to the copy
    return newBoard; // return the new board state
}

bool Board::canCastleKingSide(Color c) const {
    if (sideToMove == WHITE) {
        return (hasWK() && // king or rook has not moved
                !(occAll & ( BB(F1) | BB(G1) ) ) && // squares between king and rook are empty  
                !isSquareAttacked(E1, BLACK) && // king not in check
                !isSquareAttacked(F1, BLACK) && // king not passing through check
                !isSquareAttacked(G1, BLACK)); // king not moving into check 

    } else { // black to move
        return (hasBK() && // king or rook has not moved
                !(occAll & ( BB(F8) | BB(G8) ) ) && // squares between king and rook are empty  
                !isSquareAttacked(E8, WHITE) && // king not in check
                !isSquareAttacked(F8, WHITE) && // king not passing through check
                !isSquareAttacked(G8, WHITE)); // king not moving into check
    }
}

bool Board::canCastleQueenSide(Color c) const {
    if (c == WHITE) {
        return (hasWQ() && // king or rook has not moved
                !(occAll & ( BB(D1) | BB(C1) | BB(B1) ) ) && // squares between king and rook are empty  
                !isSquareAttacked(E1, BLACK) && // king not in check
                !isSquareAttacked(D1, BLACK) && // king not passing through check
                !isSquareAttacked(C1, BLACK)); // king not moving into check 

    } else { // black to move
        return (hasBQ() && // king or rook has not moved
                !(occAll & ( BB(D8) | BB(C8) | BB(B8) ) ) && // squares between king and rook are empty
                !isSquareAttacked(E8, WHITE) && // king not in check
                !isSquareAttacked(D8, WHITE) && // king not passing through check
                !isSquareAttacked(C8, WHITE)); // king not moving into check
    }
}

vector<Move> Board::generateMoves() const {
    std::vector<Move> moves;
    for (int p = 0; p < PIECE_N; ++p) {
        U64 pieces = bb[sideToMove][p];
        while (pieces) {
            U64 pieceBoard = pieces & -pieces; // get lowest set bit
            int sq = getSquare(pieceBoard);
            // call coresponding function from function array
            pieceFunctionArray[p](moves, pieceBoard, sq, sideToMove, *this); // appends generated moves to moves vector
            pieces &= pieces - 1; // clear lowest set bit
        }
    }
    return moves;
}

void Board::push_promos(std::vector<Move>& moves, int from, int to, U16 baseFlags) {
    Move moveQ;
    moveQ.from = from;
    moveQ.to = to;
    moveQ.flags = baseFlags | MF_PromoQ;
    moveQ.piece = PAWN;
    moves.push_back(moveQ);

    Move moveR;
    moveR.from = from;
    moveR.to = to;
    moveR.flags = baseFlags | MF_PromoR;
    moveR.piece = PAWN;
    moves.push_back(moveR);

    Move moveB;
    moveB.from = from;
    moveB.to = to;
    moveB.flags = baseFlags | MF_PromoB;
    moveB.piece = PAWN;
    moves.push_back(moveB);

    Move moveN;
    moveN.from = from;
    moveN.to = to;
    moveN.flags = baseFlags | MF_PromoN;
    moveN.piece = PAWN;
    moves.push_back(moveN);
} 

void Board::genPawnMoves(std::vector<Move>& moves, U64 pawnBB, int sq, Color c, const Board& board) {
    if (c == WHITE) {
        if ((pawnBB << 8) & ~board.occAll) { // single push
            int toSq = sq + 8;
            if (pawnBB & RANK_7) {
                // promotion moves
                push_promos(moves, sq, toSq, MF_None);
            } else {
                Move move;
                move.from = sq;
                move.to = toSq;
                move.flags = MF_None;
                move.piece = PAWN;
                moves.push_back(move);
            }

            if ( (pawnBB & RANK_2) && ( (pawnBB << 16) & ~board.occAll ) ) { // double push from rank 2
                Move move2;
                move2.from = sq;
                move2.to = sq + 16;
                move2.flags = MF_DoublePush;
                move2.piece = PAWN;
                moves.push_back(move2);
            }
        }

        //captures
        if ((pawnBB & ~FILE_A) << 7 & board.occ[other(c)]) { // capture to the left
            int toSq = sq + 7;
            U64 toBB = BB(toSq);
            if (toBB & RANK_8) { 
                // promotion capture moves
                push_promos(moves, sq, toSq, MF_Capture);
            } else {
            Move move;
            move.from = sq;
            move.to = toSq;
            move.flags = MF_Capture;
            move.piece = PAWN;
            moves.push_back(move);
            }
        }
        if ((pawnBB & ~FILE_H) << 9 & board.occ[other(c)]) { // capture to the right
            int toSq = sq + 9;
            U64 toBB = BB(toSq);
            if (toBB & RANK_8) { 
                // promotion capture moves
                push_promos(moves, sq, toSq, MF_Capture);
            } else {
            Move move;
            move.from = sq;
            move.to = toSq;
            move.flags = MF_Capture;
            move.piece = PAWN;
            moves.push_back(move);
            }
        }

    } else { // black to move
        if ((pawnBB >> 8) & ~board.occAll) { // single push
            int toSq = sq - 8;
            if (pawnBB & RANK_2) { 
                // promotion moves
                push_promos(moves, sq, toSq, MF_None);
            } else {
                Move move;
                move.from = sq;
                move.to = toSq;
                move.flags = MF_None;
                move.piece = PAWN;
                moves.push_back(move);
            }

            if ((pawnBB & RANK_7) && ( (pawnBB >> 16) & ~board.occAll )) { // double push from rank 7
                Move move2;
                move2.from = sq;
                move2.to = sq - 16;
                move2.flags = MF_DoublePush;
                move2.piece = PAWN;
                moves.push_back(move2);
            }
        }
        //captures
        if ((pawnBB & ~FILE_A) >> 9 & board.occ[other(c)]) { // capture to black players right
            int toSq = sq - 9;
            U64 toBB = BB(toSq);
            if (toBB & RANK_1) { 
                // promotion capture moves
                push_promos(moves, sq, toSq, MF_Capture);
            } else {
            Move move;
            move.from = sq;
            move.to = toSq;
            move.flags = MF_Capture;
            move.piece = PAWN;
            moves.push_back(move);
            }

        }
        if ((pawnBB & ~FILE_H) >> 7 & board.occ[other(c)]) { // capture to black players left
            int toSq = sq - 7;
            U64 toBB = BB(toSq);
            if (toBB & RANK_1) { 
                // promotion capture moves
                push_promos(moves, sq, toSq, MF_Capture);
            } else {
                Move move;
                move.from = sq;
                move.to = toSq;
                move.flags = MF_Capture;
                move.piece = PAWN;
                moves.push_back(move);
            }
        }
    }
}

void Board::genKnightMoves(std::vector<Move>& moves, U64 knightBB, int sq, Color c, const Board& board) {
    const U64 own = board.occ[c];

    // original computing knight attacks every time
    // U64 from = knightBB;
    // U64 attack  = 0;

    // attack |= (from & ~FILE_H)  << 17;
    // attack |= (from & ~FILE_A)  << 15;
    // attack |= (from & ~FILE_GH) << 10;
    // attack |= (from & ~FILE_AB) <<  6;
    // attack |= (from & ~FILE_H)  >> 15;
    // attack |= (from & ~FILE_A)  >> 17;
    // attack |= (from & ~FILE_GH) >>  6;
    // attack |= (from & ~FILE_AB) >> 10;

    // U64 targets = attack & ~own;

    // using precomputed knight attack targets defined in types.hpp
    U64 targets = KNIGHT_ATTACK_TARGETS[sq] & ~own;

    while (targets) {
        U64 toBB = targets & -targets;
        int to   = getSquare(toBB);
        const U16 flag = (toBB & board.occ[other(c)]) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = to;
        move.flags = flag;
        move.piece = KNIGHT;
        moves.push_back(move);
        targets ^= toBB;
    }
}

void Board::genDiagonalMoves(std::vector<Move>& moves, U64 pieceBB, int sq, Color c, const Board& board, Piece piece) {
    const U64 own = board.occ[c];
    const U64 enemy = board.occ[other(c)];

    U64 p = pieceBB;

    // north-east
    while(( p = ( (p & ~FILE_H & ~RANK_8) << 9 ) )) { 
        if (p & own) break; 
        const U16 flag = (p & enemy) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = getSquare(p);
        move.flags = flag;
        move.piece = piece;
        moves.push_back(move);
        if (flag == MF_Capture) break; // stop if we captured
    }

    // north-west
    p = pieceBB;
    while(( p = ( (p & ~FILE_A & ~RANK_8) << 7 ) )) { 
        if (p & own) break; 
        const U16 flag = (p & enemy) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = getSquare(p);
        move.flags = flag;
        move.piece = piece;
        moves.push_back(move);
        if (flag == MF_Capture) break; // stop if we captured
    }

    // south-east
    p = pieceBB;
    while(( p = ( (p & ~FILE_H & ~RANK_1) >> 7 ) )){ 
        if (p & own) break; 
        const U16 flag = (p & enemy) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = getSquare(p);
        move.flags = flag;
        move.piece = piece;
        moves.push_back(move);
        if (flag == MF_Capture) break; // stop if we captured
    }

    // south-west
    p = pieceBB;
    while(( p = ( (p & ~FILE_A & ~RANK_1) >> 9 ) )) { 
        if (p & own) break; 
        const U16 flag = (p & enemy) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = getSquare(p);
        move.flags = flag;
        move.piece = piece;
        moves.push_back(move);
        if (flag == MF_Capture) break; // stop if we captured
    }
}

void Board::genStraightMoves(std::vector<Move>& moves, U64 pieceBB, int sq, Color c, const Board& board, Piece piece) {
    const U64 own = board.occ[c];
    const U64 enemy = board.occ[other(c)];

    U64 p = pieceBB;

    // north
    while(( p = ( (p & ~RANK_8) << 8 ) )) {
        if (p & own) break; 
        const U16 flag = (p & enemy) ? MF_Capture : MF_None;
        
        Move move;
        move.from = sq;
        move.to = getSquare(p);
        move.flags = flag;
        move.piece = piece;
        moves.push_back(move);
        if (flag == MF_Capture) break; // stop if we captured
    }

    // south
    p = pieceBB;
    while(( p = ( (p & ~RANK_1) >> 8 ) )) {
        if (p & own) break; 
        const U16 flag = (p & enemy) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = getSquare(p);
        move.flags = flag;
        move.piece = piece;
        moves.push_back(move);
        if (flag == MF_Capture) break; // stop if we captured
    }

    // east
    p = pieceBB;
    while(( p = ( (p & ~FILE_H) << 1 ) )) {
        if (p & own) break; 
        const U16 flag = (p & enemy) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = getSquare(p);
        move.flags = flag;
        move.piece = piece;
        moves.push_back(move);
        if (flag == MF_Capture) break; // stop if we captured
    }

    // west
    p = pieceBB;
    while(( p = ( (p & ~FILE_A) >> 1 ) )) {
        if (p & own) break; 
        const U16 flag = (p & enemy) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = getSquare(p);
        move.flags = flag;
        move.piece = piece;
        moves.push_back(move);
        if (flag == MF_Capture) break; // stop if we captured
    }
}

void Board::genBishopMoves(std::vector<Move>& moves, U64 bishopBB, int sq, Color c, const Board& board) {
    genDiagonalMoves(moves, bishopBB, sq, c, board, BISHOP);
}

void Board::genRookMoves(std::vector<Move>& moves, U64 rookBB, int sq, Color c, const Board& board) {
    genStraightMoves(moves, rookBB, sq, c, board, ROOK);
}

void Board::genQueenMoves(std::vector<Move>& moves, U64 queenBB, int sq, Color c, const Board& board) {
    genDiagonalMoves(moves, queenBB, sq, c, board, QUEEN);
    genStraightMoves(moves, queenBB, sq, c, board, QUEEN);  
}

void Board::genKingMoves(std::vector<Move>& moves, U64 kingBB, int sq, Color c, const Board& board) {
    const U64 own = board.occ[c];

    U64 targets = KING_ATTACK_TARGETS[sq] & ~own;

    while (targets) {
        U64 toBB = targets & -targets;
        int to   = getSquare(toBB);
        const U16 flag = (toBB & board.occ[other(c)]) ? MF_Capture : MF_None;
        Move move;
        move.from = sq;
        move.to = to;
        move.flags = flag;
        move.piece = KING;
        moves.push_back(move);
        targets ^= toBB;
    }

    // Castling moves
    if (board.canCastleKingSide(c)) {
        Square kingTo = (c == WHITE) ? G1 : G8;
        Move move;
        move.from = sq;
        move.to = kingTo;
        move.flags = MF_CastleK;
        move.piece = KING;
        moves.push_back(move);
    }
    if (board.canCastleQueenSide(c)) {
        Square kingTo = (c == WHITE) ? C1 : C8;
        Move move;
        move.from = sq;
        move.to = kingTo;
        move.flags = MF_CastleQ;
        move.piece = KING;
        moves.push_back(move);
    }
}





} // namespace chess