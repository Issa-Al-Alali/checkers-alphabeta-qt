# Checkers Game with AI

## Description

This is a Checkers (Draughts) game implemented in C++ with a graphical user interface using the Qt framework. The game includes an AI opponent powered by the Alpha-Beta pruning search algorithm to make challenging moves.

## Features

- Graphical User Interface using Qt for board representation and interaction.
- Player vs AI mode.
- AI opponent using the Alpha-Beta pruning algorithm for efficient game tree search.
- Implementation of standard Checkers rules (movement, capturing, crowning kings).

## Technologies Used

- **Programming Language:** C++
- **GUI Framework:** Qt
- **AI Algorithm:** Alpha-Beta Pruning

## Setup & Installation

1.  Create a Qt Widgets Application and copy then paste the main.cpp
2.  Ensure you have a compatible C++ compiler (like g++ or Clang) and the Qt development libraries installed.

## How to Play

1.  Launch the application.
2.  Players take turns moving their pieces diagonally forward.
3.  To capture an opponent's piece, jump over it diagonally to an empty square immediately beyond it. Multiple jumps can be made in a single turn if possible.
4.  If a piece reaches the opponent's back row, it becomes a "King" and can move diagonally forwards and backwards.
5.  The game ends when one player captures all of the opponent's pieces or blocks them from making any valid moves.

## Contributors

- Issa Al Alali / Issa-Al-Alali
- Georges Sayde / georges618
- Hassan Moaullem / Hassan-ha

## License

MIT License
