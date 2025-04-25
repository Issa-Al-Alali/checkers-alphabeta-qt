#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <vector>
#include <limits>
#include <string>
#include <cmath> // Required for abs()
#include <QDebug> // Required for qDebug()

using namespace std;

// Define pieces
const char WHITE_PIECE = 'w';
const char WHITE_KING = 'W';
const char BLACK_PIECE = 'b';
const char BLACK_KING = 'B';
const char EMPTY_SQUARE = ' ';

// Define players
const char WHITE = 'W'; // AI is White
const char BLACK = 'B'; // Human is Black

// Structure to represent a move
struct Move {
    int startRow, startCol;
    int endRow, endCol;
    bool isCapture; // Indicates if this move is a capture
    vector<pair<int, int>> capturedPieces; // Coordinates of captured pieces
};

// Structure to represent a single square's coordinates
struct Square {
    int row, col;
};

// --- Simplified Checkers Logic ---

// Basic board representation (8x8, but only dark squares are playable)
using CheckersBoard = vector<vector<char>>;

// Function to initialize a standard Checkers starting board
CheckersBoard initializeBoard() {
    CheckersBoard board(8, vector<char>(8, EMPTY_SQUARE));

    // Place Black pieces
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 8; ++j) {
            if ((i + j) % 2 != 0) { // Only place on dark squares
                board[i][j] = BLACK_PIECE;
            }
        }
    }

    // Place White pieces
    for (int i = 5; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if ((i + j) % 2 != 0) { // Only place on dark squares
                board[i][j] = WHITE_PIECE;
            }
        }
    }

    return board;
}

// Helper function to check if a square is on the board and is a dark square
bool isValidSquare(int row, int col) {
    return row >= 0 && row < 8 && col >= 0 && col < 8 && (row + col) % 2 != 0;
}

// Simplified function to check if a move is valid (basic movement and capture)
// This does NOT fully implement all Checkers rules (e.g., mandatory multi-jumps)
bool isValidMove(const CheckersBoard& board, const Move& move, char currentPlayer) {
    // Check if start and end squares are valid dark squares
    if (!isValidSquare(move.startRow, move.startCol) || !isValidSquare(move.endRow, move.endCol)) {
        return false;
    }

    char piece = board[move.startRow][move.startCol];
    char targetPiece = board[move.endRow][move.endCol];

    // Check if the piece belongs to the current player
    bool isCurrentPlayerPiece = false;
    if (currentPlayer == WHITE) {
        if (piece == WHITE_PIECE || piece == WHITE_KING) isCurrentPlayerPiece = true;
    }
    else { // Black player
        if (piece == BLACK_PIECE || piece == BLACK_KING) isCurrentPlayerPiece = true;
    }
    if (!isCurrentPlayerPiece) return false;

    // Cannot move to a non-empty square
    if (targetPiece != EMPTY_SQUARE) return false;

    int rowDiff = move.endRow - move.startRow;
    int colDiff = move.endCol - move.startCol;

    // --- Basic Movement and Capture Logic (Simplified) ---

    // Regular piece movement (one step diagonally forward)
    if (piece == WHITE_PIECE) {
        if (rowDiff == -1 && abs(colDiff) == 1) return true;
    }
    else if (piece == BLACK_PIECE) {
        if (rowDiff == 1 && abs(colDiff) == 1) return true;
    }

    // King movement (one step diagonally in any direction)
    if (piece == WHITE_KING || piece == BLACK_KING) {
        if (abs(rowDiff) == 1 && abs(colDiff) == 1) return true;
    }

    // Basic Capture (jumping over one opponent piece)
    if (abs(rowDiff) == 2 && abs(colDiff) == 2) {
        int jumpedRow = move.startRow + rowDiff / 2;
        int jumpedCol = move.startCol + colDiff / 2;

        if (isValidSquare(jumpedRow, jumpedCol)) {
            char jumpedPiece = board[jumpedRow][jumpedCol];
            // Check if the jumped piece is an opponent's piece
            if (currentPlayer == WHITE) {
                if (jumpedPiece == BLACK_PIECE || jumpedPiece == BLACK_KING) return true;
            }
            else { // Black player
                if (jumpedPiece == WHITE_PIECE || jumpedPiece == WHITE_KING) return true;
            }
        }
    }

    // If none of the above, the move is not valid in this simplified model
    return false;
}

// Function to generate all possible legal moves for the current player (simplified)
vector<Move> generateLegalMoves(const CheckersBoard& board, char currentPlayer) {
    vector<Move> legalMoves;
    vector<Move> captureMoves; // Prioritize capture moves

    for (int startRow = 0; startRow < 8; ++startRow) {
        for (int startCol = 0; startCol < 8; ++startCol) {
            if (!isValidSquare(startRow, startCol)) continue;

            char piece = board[startRow][startCol];
            bool isCurrentPlayerPiece = false;
            if (currentPlayer == WHITE) {
                if (piece == WHITE_PIECE || piece == WHITE_KING) isCurrentPlayerPiece = true;
            }
            else { // Black player
                if (piece == BLACK_PIECE || piece == BLACK_KING) isCurrentPlayerPiece = true;
            }
            if (!isCurrentPlayerPiece) continue;

            // Check possible diagonal moves (1 or 2 steps for captures)
            int rowDirections[] = { -1, 1 };
            int colDirections[] = { -1, 1 };

            for (int rowDir : rowDirections) {
                for (int colDir : colDirections) {
                    // Check for single step moves
                    int endRow = startRow + rowDir;
                    int endCol = startCol + colDir;
                    Move singleStepMove = { startRow, startCol, endRow, endCol, false, {} };
                    if (isValidMove(board, singleStepMove, currentPlayer)) {
                        // Regular pieces can only move forward
                        if ((piece == WHITE_PIECE && rowDir == -1) || (piece == BLACK_PIECE && rowDir == 1) || piece == WHITE_KING || piece == BLACK_KING) {
                            legalMoves.push_back(singleStepMove);
                        }
                    }

                    // Check for capture moves (two steps)
                    endRow = startRow + rowDir * 2;
                    endCol = startCol + colDir * 2;
                    Move captureMove = { startRow, startCol, endRow, endCol, true, {} };
                    if (isValidMove(board, captureMove, currentPlayer)) {
                        int jumpedRow = startRow + rowDir;
                        int jumpedCol = startCol + colDir;
                        captureMove.capturedPieces.push_back({ jumpedRow, jumpedCol });
                        captureMoves.push_back(captureMove);
                    }
                }
            }
        }
    }

    // In Checkers, captures are mandatory. If capture moves exist, only return those.
    if (!captureMoves.empty()) {
        qDebug() << "Generated capture moves for" << currentPlayer << ":" << captureMoves.size();
        return captureMoves;
    }

    qDebug() << "Generated legal moves for" << currentPlayer << ":" << legalMoves.size();
    return legalMoves;
}


// Simplified evaluation function (based on piece count and king count)
// Positive value favors White (AI), negative favors Black (Human)
int evaluateBoard(const CheckersBoard& board) {
    int score = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == WHITE_PIECE) score += 1;
            else if (board[i][j] == WHITE_KING) score += 3; // Kings are more valuable
            else if (board[i][j] == BLACK_PIECE) score -= 1;
            else if (board[i][j] == BLACK_KING) score -= 3; // Kings are more valuable
        }
    }
    return score;
}

// Function to apply a move to the board
CheckersBoard applyMove(const CheckersBoard& board, const Move& move) {
    CheckersBoard newBoard = board;
    char movedPiece = newBoard[move.startRow][move.startCol];

    // Move the piece
    newBoard[move.endRow][move.endCol] = movedPiece;
    newBoard[move.startRow][move.startCol] = EMPTY_SQUARE;

    // Remove captured pieces
    for (const auto& captured : move.capturedPieces) {
        newBoard[captured.first][captured.second] = EMPTY_SQUARE;
    }

    // Promote to King if a piece reaches the opposite end
    if (movedPiece == WHITE_PIECE && move.endRow == 0) {
        newBoard[move.endRow][move.endCol] = WHITE_KING;
    }
    else if (movedPiece == BLACK_PIECE && move.endRow == 7) {
        newBoard[move.endRow][move.endCol] = BLACK_KING;
    }

    return newBoard;
}


// The Alpha-Beta pruning algorithm function
// alpha: Best value Maximizer (AI - White) can guarantee so far
// beta: Best value Minimizer (Human - Black) can guarantee so far
// depth: Current search depth
// maxDepth: Maximum search depth
int alphaBeta(CheckersBoard& board, int depth, int maxDepth, bool isMaximizingPlayer, int alpha, int beta) {
    // Base case: If max depth is reached or no legal moves for the current player
    char currentPlayer = isMaximizingPlayer ? WHITE : BLACK;
    vector<Move> possibleMoves = generateLegalMoves(board, currentPlayer);

    if (depth == maxDepth || possibleMoves.empty()) {
        return evaluateBoard(board);
    }


    if (isMaximizingPlayer) { // White's turn (AI)
        int best = numeric_limits<int>::min();

        for (const auto& move : possibleMoves) {
            // Apply the move
            CheckersBoard newBoard = applyMove(board, move);

            // Recurse
            int value = alphaBeta(newBoard, depth + 1, maxDepth, false, alpha, beta);

            best = max(best, value);
            alpha = max(alpha, best);

            // Alpha Beta Pruning
            if (beta <= alpha) {
                break;
            }
        }
        return best;
    }
    else { // Black's turn (Human)
        int best = numeric_limits<int>::max();

        for (const auto& move : possibleMoves) {
            // Apply the move
            CheckersBoard newBoard = applyMove(board, move);

            // Recurse
            int value = alphaBeta(newBoard, depth + 1, maxDepth, true, alpha, beta);

            best = min(best, value);
            beta = min(beta, best);

            // Alpha Beta Pruning
            if (beta <= alpha) {
                break;
            }
        }
        return best;
    }
}

// Function to find the best move for the AI using Alpha-Beta
Move findBestMove(CheckersBoard& board, int maxDepth) {
    int bestVal = numeric_limits<int>::min();
    Move bestMove = { -1, -1, -1, -1, false, {} }; // Indicate no valid move found yet

    int alpha = numeric_limits<int>::min();
    int beta = numeric_limits<int>::max();

    vector<Move> possibleMoves = generateLegalMoves(board, WHITE); // AI is White

    for (const auto& move : possibleMoves) {
        // Apply the move
        CheckersBoard newBoard = applyMove(board, move);

        // Compute evaluation function for this move with Alpha-Beta
        int moveVal = alphaBeta(newBoard, 0, maxDepth, false, alpha, beta); // After AI moves, it's Human's turn (minimizing)

        // If the value of the current move is more than the best value, then update best
        if (moveVal > bestVal) {
            bestVal = moveVal;
            bestMove = move;
        }
        // Update alpha for the top-level call (maximizing player's turn)
        alpha = max(alpha, moveVal);
    }
    qDebug() << "Best AI move found:" << bestMove.startRow << bestMove.startCol << "to" << bestMove.endRow << bestMove.endCol << "Is Capture:" << bestMove.isCapture;
    return bestMove;
}


// --- Qt GUI Implementation ---

class CheckersWindow : public QMainWindow {
    Q_OBJECT // Add this macro

public:
    CheckersWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Simplified Checkers with Alpha-Beta");
        setFixedSize(640, 700); // Adjust size for 8x8 board

        QWidget* centralWidget = new QWidget(this);
        QGridLayout* gridLayout = new QGridLayout(centralWidget);
        centralWidget->setLayout(gridLayout);
        setCentralWidget(centralWidget);

        // Create the 8x8 board buttons
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                QPushButton* button = new QPushButton("", centralWidget);
                button->setFixedSize(80, 80); // Fixed size for squares
                button->setFont(QFont("Arial", 30)); // Font size for pieces
                // Set background color for chessboard pattern
                if ((i + j) % 2 != 0) { // Dark squares are playable
                    button->setStyleSheet("background-color: #B58863; color: white;"); // Dark squares
                    button->setEnabled(true); // Enable dark squares
                }
                else {
                    button->setStyleSheet("background-color: #F0D9B5;"); // Light squares
                    button->setEnabled(false); // Disable light squares
                }
                gridLayout->addWidget(button, i, j);
                boardButtons[i][j] = button;

                // Connect button click to the handleButtonClick slot
                connect(button, &QPushButton::clicked, this, [=]() {
                    handleSquareClick(i, j);
                    });
            }
        }

        // Status label
        statusLabel = new QLabel("White's turn (AI)", centralWidget); // AI starts as White
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setFont(QFont("Arial", 16));
        gridLayout->addWidget(statusLabel, 8, 0, 1, 8); // Span across all columns

        // Initialize game state
        gameBoard = initializeBoard();
        currentPlayer = WHITE; // AI (White) starts
        selectedSquare = { -1, -1 }; // No square selected initially
        aiSearchDepth = 5; // Set a shallow search depth for simplicity/speed

        updateBoardUI(); // Update the UI to show the initial board

        // Since AI starts, trigger its first move
        if (currentPlayer == WHITE) {
            qDebug() << "AI's turn (White) - Making first move...";
            QApplication::processEvents(); // Update UI before AI thinks
            makeAIMove();
            updateBoardUI();
            // Check game end after AI move, and only switch turn if game is not over
            if (!checkGameEnd()) {
                currentPlayer = BLACK;
                statusLabel->setText("Black's turn (Human)");
                qDebug() << "Switched to Black's turn.";
            }
            else {
                qDebug() << "Game ended after AI's first move.";
            }
        }
    }

private slots:
    // Slot to handle square clicks
    void handleSquareClick(int row, int col) {
        // Only process if it's the human player's turn (Black) and a valid dark square
        if (currentPlayer != BLACK || !isValidSquare(row, col)) return;

        if (selectedSquare.row == -1) {
            // First click: Select a piece to move
            char piece = gameBoard[row][col];
            // Check if the selected piece belongs to the human player (Black)
            if (piece == BLACK_PIECE || piece == BLACK_KING) {
                selectedSquare = { row, col };
                // Highlight the selected square (optional)
                boardButtons[row][col]->setStyleSheet(boardButtons[row][col]->styleSheet() + "border: 2px solid blue;");
                qDebug() << "Selected square:" << row << col;
            }
            else {
                qDebug() << "Selected square with no Black piece or empty:" << row << col;
            }
        }
        else {
            // Second click: Select destination square
            Move playerMove = { selectedSquare.row, selectedSquare.col, row, col, false, {} };
            qDebug() << "Attempting human move:" << playerMove.startRow << playerMove.startCol << "to" << playerMove.endRow << playerMove.endCol;

            // Check if the move is valid for the human player
            if (isValidMove(gameBoard, playerMove, BLACK)) {
                // If it's a capture move, find the captured piece
                if (abs(playerMove.startRow - playerMove.endRow) == 2) {
                    playerMove.isCapture = true;
                    int jumpedRow = playerMove.startRow + (playerMove.endRow - playerMove.startRow) / 2;
                    int jumpedCol = playerMove.startCol + (playerMove.endCol - playerMove.startCol) / 2;
                    playerMove.capturedPieces.push_back({ jumpedRow, jumpedCol });
                }

                // Apply the player's move
                gameBoard = applyMove(gameBoard, playerMove);

                updateBoardUI(); // Update the UI
                qDebug() << "Human move made.";

                // Check for game end after player move
                if (checkGameEnd()) {
                    selectedSquare = { -1, -1 }; // Reset selection
                    return; // Game is over
                }

                // Simplified multi-jump check: If a capture was made, check if more captures are possible from the new position
                // A full implementation would enforce mandatory multi-jumps.
                if (playerMove.isCapture) {
                    vector<Move> possibleNextCaptures = generateLegalMoves(gameBoard, BLACK);
                    bool canMultiJump = false;
                    for (const auto& nextMove : possibleNextCaptures) {
                        if (nextMove.isCapture && nextMove.startRow == playerMove.endRow && nextMove.startCol == playerMove.endCol) {
                            canMultiJump = true;
                            break;
                        }
                    }
                    if (canMultiJump) {
                        statusLabel->setText("Black's turn (Human) - Multi-jump!");
                        // Keep currentPlayer as BLACK for multi-jump
                        // The player needs to select the same piece again for the next jump
                        selectedSquare = { playerMove.endRow, playerMove.endCol }; // Keep the piece selected
                        boardButtons[selectedSquare.row][selectedSquare.col]->setStyleSheet(boardButtons[selectedSquare.row][selectedSquare.col]->styleSheet() + "border: 2px solid blue;");
                        return; // Stay in human's turn for multi-jump
                    }
                }


                // Switch to AI's turn if no multi-jump
                currentPlayer = WHITE;
                statusLabel->setText("White's turn (AI)");
                qDebug() << "Switched to White's turn.";

                // Trigger AI's move
                QApplication::processEvents(); // Update UI before AI thinks
                makeAIMove();
                updateBoardUI();

                // Check for game end after AI move
                checkGameEnd();

                // Switch back to player's turn if game is not over and turn is still AI's
                if (currentPlayer == WHITE) {
                    currentPlayer = BLACK;
                    statusLabel->setText("Black's turn (Human)");
                    qDebug() << "Switched back to Black's turn.";
                }
                else {
                    qDebug() << "Game ended after AI's move.";
                }


            }
            else {
                // Invalid move
                QMessageBox::warning(this, "Invalid Move", "That is not a valid move.");
                qDebug() << "Invalid human move.";
            }

            // Reset selection and remove highlight
            if (selectedSquare.row != -1) {
                boardButtons[selectedSquare.row][selectedSquare.col]->setStyleSheet(
                    (selectedSquare.row + selectedSquare.col) % 2 != 0 ? "background-color: #B58863; color: white;" : "background-color: #F0D9B5;"
                );
            }
            selectedSquare = { -1, -1 };
        }
    }

    // Function to update the board UI based on the gameBoard state
    void updateBoardUI() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if ((i + j) % 2 != 0) { // Only update dark squares
                    boardButtons[i][j]->setText(QString(gameBoard[i][j]));
                }
                else {
                    boardButtons[i][j]->setText(""); // Ensure light squares are empty
                }
            }
        }
    }

    // Function to make the AI's move using Alpha-Beta
    void makeAIMove() {
        statusLabel->setText("White's turn (AI) - Thinking...");
        QApplication::processEvents(); // Update status label
        qDebug() << "AI thinking...";

        Move aiMove = findBestMove(gameBoard, aiSearchDepth);

        if (aiMove.startRow != -1) { // Check if a valid move was found
            // Apply the AI's move
            gameBoard = applyMove(gameBoard, aiMove);
            qDebug() << "AI made move:" << aiMove.startRow << aiMove.startCol << "to" << aiMove.endRow << aiMove.endCol << "Is Capture:" << aiMove.isCapture;

            // Simplified multi-jump check for AI
            if (aiMove.isCapture) {
                vector<Move> possibleNextCaptures = generateLegalMoves(gameBoard, WHITE);
                bool canMultiJump = false;
                for (const auto& nextMove : possibleNextCaptures) {
                    if (nextMove.isCapture && nextMove.startRow == aiMove.endRow && nextMove.startCol == aiMove.endCol) {
                        canMultiJump = true;
                        break;
                    }
                }
                if (canMultiJump) {
                    statusLabel->setText("White's turn (AI) - Multi-jump!");
                    QApplication::processEvents(); // Update status label
                    makeAIMove(); // Trigger another AI move for multi-jump
                    return; // AI stays in turn for multi-jump
                }
            }

        }
        else {
            // No valid move found for AI (shouldn't happen in a normal game unless game over)
            statusLabel->setText("AI has no legal moves.");
            qDebug() << "AI found no legal moves.";
        }
    }

    // Function to check if the game has ended (simplified: checks if a player has no pieces or no legal moves)
    bool checkGameEnd() {
        bool whitePieceExists = false;
        bool blackPieceExists = false;

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (gameBoard[i][j] == WHITE_PIECE || gameBoard[i][j] == WHITE_KING) whitePieceExists = true;
                if (gameBoard[i][j] == BLACK_PIECE || gameBoard[i][j] == BLACK_KING) blackPieceExists = true;
            }
        }

        if (!whitePieceExists) {
            statusLabel->setText("Black Wins!");
            disableAllButtons();
            QMessageBox::information(this, "Game Over", "Black Wins!");
            currentPlayer = EMPTY_SQUARE; // Indicate game over
            qDebug() << "Game Over: Black Wins!";
            return true;
        }
        else if (!blackPieceExists) {
            statusLabel->setText("White Wins!");
            disableAllButtons();
            QMessageBox::information(this, "Game Over", "White Wins!");
            currentPlayer = EMPTY_SQUARE; // Indicate game over
            qDebug() << "Game Over: White Wins!";
            return true;
        }

        // Check if the current player has any legal moves
        if (generateLegalMoves(gameBoard, currentPlayer).empty()) {
            if (currentPlayer == WHITE) {
                statusLabel->setText("Black Wins (White has no moves)!");
                QMessageBox::information(this, "Game Over", "Black Wins (White has no moves)!");
            }
            else {
                statusLabel->setText("White Wins (Black has no moves)!");
                QMessageBox::information(this, "Game Over", "White Wins (Black has no moves)!");
            }
            disableAllButtons();
            currentPlayer = EMPTY_SQUARE; // Indicate game over
            qDebug() << "Game Over: No legal moves for current player.";
            return true;
        }


        return false;
    }

    // Function to disable all buttons after the game ends
    void disableAllButtons() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                boardButtons[i][j]->setEnabled(false);
            }
        }
    }


private:
    QPushButton* boardButtons[8][8]; // 2D array of buttons for the board
    CheckersBoard gameBoard; // Game board state
    char currentPlayer; // 'W' or 'B'
    Square selectedSquare; // Stores the coordinates of the selected piece
    int aiSearchDepth; // Depth for Alpha-Beta search
    QLabel* statusLabel; // Label to display game status
};

#include "main.moc" // Include the generated moc file

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    CheckersWindow w;
    w.show();
    return a.exec();
}
