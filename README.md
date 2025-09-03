# Chess Engine (in development)

Bitboard-based chess engine project. Positions are represented with 64-bit integers; move generation uses bitwise operations for speed. **Evaluation** is the next milestone.

## Features

- 64-bit bitboards for all pieces and occupancy
- Legal move generation: pawns (double pushes, captures, **en passant**), knights, bishops, rooks, queen, king, **castling**, **promotions**
- King-safety filtering (no illegal checks)
- FEN load/save
- Perft with breakdown: **Nodes, Captures, En Passant, Castles, Promotions, Checks, Checkmates**
- Simple board/bitboard printers

## Quick start

**Build**
```
make
```
**Run interactive demo (enter a FEN, prints all legal moves):**
```
make run
```

**Perft check against known positions in tests/data/perft_cases.txt:**
```
make depth5        # depth 5
# or:
make depth5time    # depth 5 with timing
```
**Performance on my machine**
```
1: 0 ms  nodes=20  nps=0
D2: 0 ms  nodes=400  nps=0
D3: 0 ms  nodes=8902  nps=0
D4: 18 ms  nodes=197281  nps=10960055
D5: 461 ms  nodes=4865609  nps=10554466
[PASS] position1
  D1: 0 ms  nodes=48  nps=0
  D2: 0 ms  nodes=2039  nps=0
  D3: 16 ms  nodes=97862  nps=6116375
  D4: 380 ms  nodes=4085603  nps=10751586
  D5: 20523 ms  nodes=193690690  nps=9437737
[PASS] position2
  D1: 0 ms  nodes=14  nps=0
  D2: 0 ms  nodes=191  nps=0
  D3: 0 ms  nodes=2812  nps=0
  D4: 9 ms  nodes=43238  nps=4804222
  D5: 100 ms  nodes=674624  nps=6746240
[PASS] position3
  D1: 0 ms  nodes=6  nps=0
  D2: 0 ms  nodes=264  nps=0
  D3: 0 ms  nodes=9467  nps=0
  D4: 48 ms  nodes=422333  nps=8798604
  D5: 1763 ms  nodes=15833292  nps=8980880
[PASS] position4
Summary: pass=4 fail=0
```

**Layout**
```
include/chess/  -> headers (defs, move, board, fen, debug, perft)
src/            -> implementation (board, fen, debug, perft, main)
tests/          -> perft checker + data
```

**Status / next steps**

-**Current: move generation complete, perft validated**

-**Next: evaluation (material + PST, then small NN), fixed-depth search with alpha–beta**