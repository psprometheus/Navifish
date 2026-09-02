<div align="center">
  <img src="logo.png" alt="Navifish Logo" width="250">
  <h3>Navifish</h3>
  <p>A strong UCI chess engine</p>
</div>

# ♟️ Navifish

**[Navifish]** is an open-source UCI (Universal Chess Interface) compatible chess engine written in C++. This engine was developed for learning about chess programming and has an estimated Elo rating of around **2500**.

*Note: [Navifish] is solely a command-line chess engine (backend). To play against it, you will need a UCI-compatible GUI such as [Arena](http://www.playwitharena.de/), [Cutechess](https://cutechess.com/), or [BanksiaGUI](https://banksiagui.com/).*

---

## 🚀 Technical Features

Below are the core algorithms and programming techniques implemented in the engine:

**Search**
* Minimax framework as well as Alpha-Beta Pruning
* Iterative Deepening
* Basic Move Ordering (MVV-LVA)
* Quiescence Search
* Transposition Table
* Butterfly history heuristic
* Principle Variant Search
* Aspiration windows
* Reverse Futility Pruning
* Null Move Pruning
* Late Move Reduction
* Killer moves
* Late Move Pruning
* Futility pruning
* Internal Iterative Reduction
* Improving heuristic

**Evaluation**
* Navifish currently uses [PeSTO's Evaluation Function](https://chessprogramming.org/PeSTO's_Evaluation_Function)

---

## 🛠️ Build Instructions

### Requirements
* Your CPU must support AVX2 and BMI2, since I haven't supported a general build yet ;)
* A compiler supporting `C++17` (GCC, Clang, or MSVC)
* `Make` or `CMake`

### Compiling from Source (Linux/macOS)
```bash
git clone git@github.com:psprometheus/Navifish.git
cd Navifish
mkdir build && cd build
cmake ..
make
```
And done, the executable will be located in the 'build' folder

---

## 🙏 Acknowledgements
This project would not have been possible without the following invaluable resources:
* [Chess Programming Wiki](https://chessprogramming.org/).
* Stockfish's Discord Server.
* [Comparison of state of the art chess sliding piece lookups](https://github.com/Gigantua/Chess_Movegen).

Huge thank to **Disservin et al.** for creating such fascinating and important components:

* [chess-library](https://github.com/Disservin/chess-library) (chess board rules, uci, etc... which is the heart of this engine).
* [fastchess](https://github.com/Disservin/fastchess) (SPRT, elo estimating).
