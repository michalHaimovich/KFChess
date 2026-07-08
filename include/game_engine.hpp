#pragma once

#include <optional>
#include <utility>
#include <iostream>
#include <vector>
#include <queue>
#include <cctype> // חובה עבור std::toupper
#include "board.hpp"

// עדכון המבנה שישמור גם את המסלול המלא
struct PendingMove {
    int startX, startY;
    int destX, destY;
    uint64_t arrivalTime;
    Board::Square piece;

    // שומר את כל המשבצות שהכלי נעל בדרך
    std::vector<std::pair<int, int>> path;

    bool operator<(const PendingMove& other) const {
        return arrivalTime > other.arrivalTime;
    }
};

class GameEngine {
private:
    Board board;
    uint64_t currentTimeMs;
    std::optional<std::pair<int, int>> selectedSquare;

    std::priority_queue<PendingMove> pendingQueue;
    std::vector<std::vector<bool>> lockedSquares;

    std::vector<std::vector<uint64_t>> jumpExpiration;

    bool isGameOver = false;

    bool isSameColor(Board::Square p1, Board::Square p2) const;
    bool isSquareLocked(int x, int y) const;

public:
    explicit GameEngine(const Board& initialBoard);

    void handleClick(int pixelX, int pixelY);
    void handleJump(int pixelX, int pixelY);
    void wait(uint64_t ms);
    void printSettledBoard() const;
};