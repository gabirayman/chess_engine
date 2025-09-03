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

// // Print a raw bitboard (bb) as 8 ranks of 8 zeros/ones (rank 8 to rank 1)
// void Board::printBB(U64 bitBoard) {
//     for (int rank = 7; rank >= 0; --rank) {
//         for (int file = 0; file < 8; ++file) {
//             int sq = rank * 8 + file;
//             U64 bit = 1ULL << sq;
//             std::cout << ((bitBoard & bit) ? '1' : '0') << ' ';
//         }
//         std::cout << '\n';
//     }
//     std::cout << std::endl;
// }


// void Board::printBoard() const {
//     for (int rank = 7; rank >= 0; --rank) {
//         std::cout << (rank + 1) << ' ';
//         for (int file = 0; file < 8; ++file) {
//             int sq = rank * 8 + file;
//             U64 m = BB(sq);

//             char ch = '.';
//             if      (bb[WHITE][PAWN]   & m) ch = 'P';
//             else if (bb[WHITE][KNIGHT] & m) ch = 'N';
//             else if (bb[WHITE][BISHOP] & m) ch = 'B';
//             else if (bb[WHITE][ROOK]   & m) ch = 'R';
//             else if (bb[WHITE][QUEEN]  & m) ch = 'Q';
//             else if (bb[WHITE][KING]   & m) ch = 'K';
//             else if (bb[BLACK][PAWN]   & m) ch = 'p';
//             else if (bb[BLACK][KNIGHT] & m) ch = 'n';
//             else if (bb[BLACK][BISHOP] & m) ch = 'b';
//             else if (bb[BLACK][ROOK]   & m) ch = 'r';
//             else if (bb[BLACK][QUEEN]  & m) ch = 'q';
//             else if (bb[BLACK][KING]   & m) ch = 'k';

//             std::cout << ch << ' ';
//         }
//         std::cout << '\n';
//     }
//     std::cout << "  a b c d e f g h\n\n";
// }

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

bool Board::isSquareAttacked(Square sq, Color by) const {
    U64 sqBB = BB(sq);

    // pawns
    U64 whitePawnAttacks = ((bb[WHITE][PAWN] & ~FILE_H) << 9) | ((bb[WHITE][PAWN] & ~FILE_A) << 7);
    U64 blackPawnAttacks = ((bb[BLACK][PAWN] & ~FILE_H) >> 7) | ((bb[BLACK][PAWN] & ~FILE_A) >> 9);

    if (by == WHITE) {
        if (sqBB & whitePawnAttacks) return true;
    } else { // by == BLACK
        if (sqBB & blackPawnAttacks) return true;
    }

    // knights
    // KNIGHT_ATTACK_TARGETS[sq] returns a bitboard of a knight's attack pattern from square sq, 
    // so if a knight of the enemy is on one of those squares, because knights attack symmetrically,
    // the square sq is attacked by that knight
    if ( KNIGHT_ATTACK_TARGETS[sq] & bb[by][KNIGHT] ) return true; 
    
    // kings
    // same for kings
    if ( KING_ATTACK_TARGETS[sq] & bb[by][KING] ) return true;

    // bishops/queens (diagonal attackers)
    U64 diagonalAttackers = bb[by][BISHOP] | bb[by][QUEEN];
    // rooks/queens (straight attackers)
    U64 straightAttackers = bb[by][ROOK] | bb[by][QUEEN];

    
    // north
    U64 p = sqBB;
    while(( p = ( (p & ~RANK_8) << 8 ) )) {
        if (p & straightAttackers) return true;
        if (p & occAll) break;
    }

    // north-east
    p = sqBB;
    while(( p = ( (p & ~FILE_H & ~RANK_8) << 9 ) )) { 
        if (p & diagonalAttackers) return true;
        if (p & occAll) break;
    }

    // east
    p = sqBB;
    while(( p = ( (p & ~FILE_H) << 1 ) )) {
        if (p & straightAttackers) return true;
        if (p & occAll) break;
    }

    // south-east
    p = sqBB;
    while(( p = ( (p & ~FILE_H & ~RANK_1) >> 7 ) )){ 
        if (p & diagonalAttackers) return true;
        if (p & occAll) break;
    } 
    
    // south
    p = sqBB;
    while(( p = ( (p & ~RANK_1) >> 8 ) )) {
        if (p & straightAttackers) return true;
        if (p & occAll) break;
    }

    // south-west
    p = sqBB;
    while(( p = ( (p & ~FILE_A & ~RANK_1) >> 9 ) )) { 
        if (p & diagonalAttackers) return true;
        if (p & occAll) break;
    }

    // west
    p = sqBB;
    while(( p = ( (p & ~FILE_A) >> 1 ) )) {
        if (p & straightAttackers) return true;
        if (p & occAll) break;
    }

    // north-west
    p = sqBB;
    while(( p = ( (p & ~FILE_A & ~RANK_8) << 7 ) )) { 
        if (p & diagonalAttackers) return true;
        if (p & occAll) break;
    }

    return false; // no attackers found
}

bool Board::isInCheck(Color c) const {
    U64 kingBB = bb[c][KING];

    Square kingSq = getSquare(kingBB);

    return isSquareAttacked(kingSq, other(c));
}

bool Board::isInCheck() const {
    U64 kingBB = bb[sideToMove][KING];

    Square kingSq = getSquare(kingBB);

    return isSquareAttacked(kingSq, other(sideToMove));
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

    if (has(move.flags, MF_DoublePush)) {
        // set en passant target square
        if (us == WHITE) {
            epTarget = BB(move.to - 8); // square behind the pawn
        } else {
            epTarget = BB(move.to + 8); // square behind the pawn
        }
    } else {
        epTarget = 0; // clear en passant target if not a double push
    }

    // handle captures
    if (has(move.flags, MF_Capture)) {
        // en passant capture
        if (has(move.flags, MF_EnPassant)) {
            if (us == WHITE) {
                U64 capturedPawnBB = BB(move.to - 8); // square of the captured pawn
                bb[BLACK][PAWN] &= ~capturedPawnBB; // remove captured pawn
            } else {
                U64 capturedPawnBB = BB(move.to + 8); // square of the captured pawn
                bb[WHITE][PAWN] &= ~capturedPawnBB; // remove captured pawn
            }
        } else { // regular capture
            Piece captured = pieceOn(move.to, them); // find captured piece
            assert(captured != PIECE_N); // there must be a piece to capture
            bb[them][captured] &= ~toBB; // remove captured piece


            // update castling rights if rook is captured
            // I think I will do a simple check if rook is on its home square when trying to castle
            // and handle it there

            // if (captured == ROOK) {
            //     if (them == WHITE) {
            //         if (toBB & BB(H1)) {
            //             castling &= ~CR_WK; // white king side rook captured
            //         } else if (toBB & BB(A1)) {
            //             castling &= ~CR_WQ; // white queen side rook captured
            //         }
            //     } else {
            //         if (toBB & BB(H8)) {
            //             castling &= ~CR_BK; // black king side rook captured
            //         } else if (toBB & BB(A8)) {
            //             castling &= ~CR_BQ; // black queen side rook captured
            //         }
            //     }
            // }
        }
    }

    // handle promotions
    if (has(move.flags, MF_PromoMask)) {
        // remove the pawn that promoted
        bb[us][PAWN] &= ~toBB;

        // add the promoted piece
        if (has(move.flags, MF_PromoQ)) {
            bb[us][QUEEN] |= toBB;
        } else if (has(move.flags, MF_PromoR)) {
            bb[us][ROOK] |= toBB;
        } else if (has(move.flags, MF_PromoB)) {
            bb[us][BISHOP] |= toBB;
        } else if (has(move.flags, MF_PromoN)) {
            bb[us][KNIGHT] |= toBB;
        } 
        // else {
        //     assert(false); // invalid promotion flag
        // }
    }

    // handle castling
    assert(!(has(move.flags, MF_CastleK) && has(move.flags, MF_CastleQ))); // can't castle both sides at once
    if (has(move.flags, MF_CastleK)) {
        if (us == WHITE) {
            // move the rook from h1 to f1
            bb[WHITE][ROOK] &= ~BB(H1);
            bb[WHITE][ROOK] |= BB(F1);
            // update castling rights
            castling &= ~(CR_WK | CR_WQ); // white king moved
        } else { // us == BLACK
            // move the rook from h8 to f8
            bb[BLACK][ROOK] &= ~BB(H8);
            bb[BLACK][ROOK] |= BB(F8);
            // update castling rights
            castling &= ~(CR_BK | CR_BQ); // black king moved
        }
    } else if (has(move.flags, MF_CastleQ)) {
        if (us == WHITE) {
            // move the rook from a1 to d1
            bb[WHITE][ROOK] &= ~BB(A1);
            bb[WHITE][ROOK] |= BB(D1);
            // update castling rights
            castling &= ~(CR_WK | CR_WQ); // white king moved
        } else { // us == BLACK
            // move the rook from a8 to d8
            bb[BLACK][ROOK] &= ~BB(A8);
            bb[BLACK][ROOK] |= BB(D8);
            // update castling rights
            castling &= ~(CR_BK | CR_BQ); // black king moved
        }
    }

    // update castling rights
    if (move.piece == KING) {
        if (us == WHITE) {
            castling &= ~(CR_WK | CR_WQ); // white king moved, remove both rights
        } else {
            castling &= ~(CR_BK | CR_BQ); // black king moved, remove both rights
        }
    } else if (move.piece == ROOK) {
        if (us == WHITE) {
            if (fromBB & BB(H1)) {
                castling &= ~CR_WK; // white king side rook moved
            } else if (fromBB & BB(A1)) {
                castling &= ~CR_WQ; // white queen side rook moved
            }
        } else {
            if (fromBB & BB(H8)) {
                castling &= ~CR_BK; // black king side rook moved
            } else if (fromBB & BB(A8)) {
                castling &= ~CR_BQ; // black queen side rook moved
            }
        }
    }

    if (move.piece == PAWN || has(move.flags, MF_Capture)) {
        halfmoveClock = 0;
    } else {
        halfmoveClock++;
    }

    // update occupancies
    recompute();
    sideToMove = them; // change turn
    if (sideToMove == WHITE) ++fullmoveNumber; 
}

Board Board::applied(const Move& move) const {
    Board newBoard = *this; // copy current board
    newBoard.applyMove(move); // apply move to the copy
    return newBoard; // return the new board state
}

bool Board::canCastleKingSide(Color c) const {
    if (c == WHITE) {
        return (hasWK() && // king or rook has not moved
                (bb[WHITE][ROOK] & BB(H1)) && // rook is on h1 (and has not been captured which is not checked in hasWK())
                !(occAll & ( BB(F1) | BB(G1) ) ) && // squares between king and rook are empty  
                !isSquareAttacked(E1, BLACK) && // king not in check
                !isSquareAttacked(F1, BLACK) && // king not passing through check
                !isSquareAttacked(G1, BLACK)); // king not moving into check 

    } else { // black to move
        return (hasBK() && // king or rook has not moved
                (bb[BLACK][ROOK] & BB(H8)) && // rook is on h8 (and has not been captured which is not checked in hasBK())
                !(occAll & ( BB(F8) | BB(G8) ) ) && // squares between king and rook are empty  
                !isSquareAttacked(E8, WHITE) && // king not in check
                !isSquareAttacked(F8, WHITE) && // king not passing through check
                !isSquareAttacked(G8, WHITE)); // king not moving into check
    }
}

bool Board::canCastleQueenSide(Color c) const {
    if (c == WHITE) {
        return (hasWQ() && // king or rook has not moved
                (bb[WHITE][ROOK] & BB(A1)) && // rook is on a1 (and has not been captured which is not checked in hasWQ())
                !(occAll & ( BB(D1) | BB(C1) | BB(B1) ) ) && // squares between king and rook are empty  
                !isSquareAttacked(E1, BLACK) && // king not in check
                !isSquareAttacked(D1, BLACK) && // king not passing through check
                !isSquareAttacked(C1, BLACK)); // king not moving into check 

    } else { // black to move
        return (hasBQ() && // king or rook has not moved
                (bb[BLACK][ROOK] & BB(A8)) && // rook is on a8 (and has not been captured which is not checked in hasBQ())
                !(occAll & ( BB(D8) | BB(C8) | BB(B8) ) ) && // squares between king and rook are empty
                !isSquareAttacked(E8, WHITE) && // king not in check
                !isSquareAttacked(D8, WHITE) && // king not passing through check
                !isSquareAttacked(C8, WHITE)); // king not moving into check
    }
}

std::vector<Move> Board::generateMoves() const {
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

std::vector<Move> Board::generateLegalMoves() const {
    std::vector<Move> legal;
    auto pseudo = generateMoves();
    legal.reserve(pseudo.size());

    const Color us = sideToMove;

    for (const Move& m : pseudo) {
        Board newBoard = *this;
        newBoard.applyMove(m);
        if (!newBoard.isInCheck(us)) {
            legal.push_back(m);
        }
    }

    return legal;
}

bool Board::hasLegalMove() const {
    auto pseudo = generateMoves();
    const Color us = sideToMove;

    for (const Move& m : pseudo) {
        Board newBoard = *this;
        newBoard.applyMove(m);
        if (!newBoard.isInCheck(us)) {
            return true; 
        }
    }
    return false; 
}

bool Board::isCheckmate() const {
    return isInCheck(sideToMove) && !hasLegalMove();
}

bool Board::isStalemate() const {
    return !isInCheck(sideToMove) && !hasLegalMove();
}

GameState Board::state() const {
    if (isCheckmate()) {
        return CHECKMATE;
    } else if (isStalemate()) {
        return STALEMATE;
    } else {
        return PLAYING;
    }
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

        // en passant captures
        if (board.hasEP()) {
            U64 enPawnBB = ( (pawnBB & ~FILE_A) << 7 ) & board.epTarget; // capture to the left
            if (enPawnBB) { 
                int toSq = getSquare(enPawnBB);
                Move move;
                move.from = sq;
                move.to = toSq;
                move.flags = MF_EnPassant | MF_Capture;
                move.piece = PAWN;
                moves.push_back(move);
            }
            enPawnBB = ( (pawnBB & ~FILE_H) << 9 ) & board.epTarget; // capture to the right
            if (enPawnBB) {
                int toSq = getSquare(enPawnBB);
                Move move;
                move.from = sq;
                move.to = toSq;
                move.flags = MF_EnPassant | MF_Capture;
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

        // en passant captures
        if (board.hasEP()) {
            U64 enPawnBB = ( (pawnBB & ~FILE_A) >> 9 ) & board.epTarget; // capture to the left
            if (enPawnBB) { 
                int toSq = getSquare(enPawnBB);
                Move move;
                move.from = sq;
                move.to = toSq;
                move.flags = MF_EnPassant | MF_Capture;
                move.piece = PAWN;
                moves.push_back(move);
            }
            enPawnBB = ( (pawnBB & ~FILE_H) >> 7 ) & board.epTarget; // capture to the right
            if (enPawnBB) {
                int toSq = getSquare(enPawnBB);
                Move move;
                move.from = sq;
                move.to = toSq;
                move.flags = MF_EnPassant | MF_Capture;
                move.piece = PAWN;
                moves.push_back(move);
            }
        }
    }
}

void Board::genKnightMoves(std::vector<Move>& moves, U64 knightBB, int sq, Color c, const Board& board) {
    // to get rid of unused variable warning
    (void)knightBB;

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
    // to get rid of unused variable warning
    (void)kingBB;

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