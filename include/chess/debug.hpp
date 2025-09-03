#pragma once
#include "chess/defs.hpp"
#include <iosfwd>

namespace chess {
struct Board;

void printBB(U64 bitboard, std::ostream& os);
void printBoard(const Board& b, std::ostream& os);

void printBB(U64 bitboard); // prints to std::cout
void printBoard(const Board& b); // prints to std::cout

} // namespace chess