#include "game_engine.hpp"
#include "pieces.hpp"
#include <cmath>


GameEngine::GameEngine(const Board& initialBoard, const EngineConfig& cfg)
    : board(initialBoard), config(cfg), currentTimeMs(0), selectedSquare(std::nullopt) {
    lockedSquares.resize(board.getHeight(), std::vector<bool>(board.getWidth(), false));
    jumpExpiration.resize(board.getHeight(), std::vector<uint64_t>(board.getWidth(), 0));
}

bool GameEngine::isSameColor(Board::Square p1, Board::Square p2) const {
    if (p1.type == '\0' || p2.type == '\0') return false;
    return p1.color == p2.color;
}

bool GameEngine::isSquareLocked(int x, int y) const {
    if (x < 0 || x >= board.getWidth() || y < 0 || y >= board.getHeight()) return false;
    return lockedSquares[y][x];
}

uint64_t GameEngine::calculateArrival(int startX, int startY, int destX, int destY) const {
    int dx = std::abs(destX - startX);
    int dy = std::abs(destY - startY);
    int distance = std::max(dx, dy);

    return currentTimeMs + static_cast<uint64_t>(distance * config.moveMsPerCell);
}

void GameEngine::handleClick(int pixelX, int pixelY) {

    if (isGameOver) {
        return;
    }
    int targetX = pixelX / config.squareSizePixels;
    int targetY = pixelY / config.squareSizePixels;

    if (targetX < 0 || targetX >= board.getWidth() || targetY < 0 || targetY >= board.getHeight()) {
        return;
    }

    if (isSquareLocked(targetX, targetY)) return;

    Board::Square targetSquare = board.at(targetX, targetY);

    if (!selectedSquare.has_value()) {
        if (targetSquare.type != '\0') {
            selectedSquare = { targetX, targetY };
        }
    }
    else {
        int startX = selectedSquare->first;
        int startY = selectedSquare->second;

        if (isSquareLocked(startX, startY)) {
            selectedSquare = std::nullopt;
            return;
        }
        Board::Square selectedPiece = board.at(startX, startY);

        // Guard: cannot start a new move if the piece is currently jumping!
        if (jumpExpiration[startY][startX] > currentTimeMs) {
            selectedSquare = std::nullopt;
            return;
        }

        if (targetSquare.type != '\0' && isSameColor(selectedPiece, targetSquare)) {
            selectedSquare = { targetX, targetY };
        }
        else {
            const Piece* pieceRule = PieceFactory::getPiece(selectedPiece.type);

            if (pieceRule != nullptr &&
                pieceRule->isValidMove(startX, startY, targetX, targetY, board)) {

                std::vector<std::pair<int, int>> movePath;
                // ... (path calculation code remains the same) ...

                bool pathBlocked = false;
                for (const auto& p : movePath) {
                    // --- Critical change: destination is not checked as blocked and not locked! ---
                    // This allows two pieces to try landing at the same place, or attacked piece to defend itself
                    if (p.first != targetX || p.second != targetY) {
                        if (isSquareLocked(p.first, p.second)) {
                            pathBlocked = true;
                            break;
                        }
                    }
                }

                if (!pathBlocked) {
                    for (const auto& p : movePath) {
                        // Lock only the path, not the final destination!
                        if (p.first != targetX || p.second != targetY) {
                            lockedSquares[p.second][p.first] = true;
                        }
                    }  

                    pendingQueue.push({
                        startX, startY, targetX, targetY,
                        calculateArrival(startX, startY, targetX, targetY),
                        selectedPiece,
                        movePath // Save the path to know what to release in wait
                    });
                }
            }

            // Reset selection at end of process
            selectedSquare = std::nullopt;
        }
    }
}

void GameEngine::handleJump(int pixelX, int pixelY) {
    if (isGameOver) return;

    int targetX = pixelX / config.squareSizePixels;
    int targetY = pixelY / config.squareSizePixels;

    if (targetX < 0 || targetX >= board.getWidth() || targetY < 0 || targetY >= board.getHeight()) {
        return;
    }

    Board::Square piece = board.at(targetX, targetY);

    // Cannot jump if there's no piece there, or if the square is locked (piece has left)
    if (piece.type == '\0' || isSquareLocked(targetX, targetY)) {
        return;
    }

    // If the piece is not jumping right now, activate jump for 1000 milliseconds
    if (jumpExpiration[targetY][targetX] <= currentTimeMs) {
        jumpExpiration[targetY][targetX] = currentTimeMs + config.jumpDurationMs;
    }

    // If the jumped piece was selected, cancel the selection (like in click)
    selectedSquare = std::nullopt;
}

void GameEngine::wait(uint64_t ms) {
    currentTimeMs += ms;

    while (!pendingQueue.empty() && pendingQueue.top().arrivalTime <= currentTimeMs) {
        PendingMove move = pendingQueue.top();
        pendingQueue.pop();

        // Check what's at the destination
        Board::Square targetSquare = board.at(move.destX, move.destY);
        bool targetIsEnemy = (targetSquare.type != '\0' && targetSquare.color != move.piece.color);

        if (targetIsEnemy && jumpExpiration[move.destY][move.destX] >= move.arrivalTime) {
            // The attacker fell into a trap! The airborne piece eats it.

            // If the destroyed attacker happens to be the king (attacked and died), game over!
            if (PieceFactory::getPiece(move.piece.type)->isVital()) {
                isGameOver = true;
            }

            // Delete the attacker from its source square (it doesn't land at destination)
            board.place(move.startX, move.startY, { '.', '\0' });
        }
        else {
            // Normal landing (no jump, or jump finished)

            // After landing the piece on the board in wait:
            board.place(move.destX, move.destY, move.piece);
            board.place(move.startX, move.startY, { '.', '\0' });

            // Tell the piece: "You landed! Do what you're supposed to do"
            PieceFactory::getPiece(move.piece.type)->onLanding(board, move.destX, move.destY, move.piece.color);

            // Game Over check (if we crushed a static king)

            if (targetSquare.type != '\0' && PieceFactory::getPiece(targetSquare.type)->isVital()) {
                isGameOver = true;
            }

        }

        // Release locks along the path (remains the same)
        for (const auto& p : move.path) {
            if (p.first != move.destX || p.second != move.destY) {
                lockedSquares[p.second][p.first] = false;
            }
        }
    }
}
void GameEngine::printSettledBoard() const {
    for (size_t y = 0; y < board.getHeight(); ++y) {
        for (size_t x = 0; x < board.getWidth(); ++x) {
            Board::Square sq = board.at(x, y);

            if (sq.type == '\0') {
                std::cout << ".";
            }
            else {
                std::cout << sq.color << sq.type;
            }

            if (x < board.getWidth() - 1) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}