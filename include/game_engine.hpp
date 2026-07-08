#pragma once

#include <optional>
#include <utility>
#include <iostream>
#include <vector>
#include <queue>
#include <cctype> 
#include "board.hpp"
#include "pieces.hpp"

struct EngineConfig {
    int squareSizePixels = 100;       // גודל משבצת להמרת קליקים
    uint64_t moveMsPerCell = 500;     // זמן תנועה למשבצת אחת
    uint64_t jumpDurationMs = 1000;   // זמן שהייה באוויר בקפיצה
};

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
    EngineConfig config;          // שומרים את ההגדרות במנוע

    uint64_t currentTimeMs;
    std::optional<std::pair<int, int>> selectedSquare;

    std::priority_queue<PendingMove> pendingQueue;
    std::vector<std::vector<bool>> lockedSquares;
    std::vector<std::vector<uint64_t>> jumpExpiration;

    bool isGameOver = false;

    bool isSameColor(Board::Square p1, Board::Square p2) const;
    bool isSquareLocked(int x, int y) const;

    // הפונקציה נכנסה פנימה כדי שתוכל לקרוא את config.moveMsPerCell
    uint64_t calculateArrival(int startX, int startY, int destX, int destY) const;

public:
    explicit GameEngine(const Board& initialBoard, const EngineConfig& cfg = EngineConfig());

    void handleClick(int pixelX, int pixelY);
    void handleJump(int pixelX, int pixelY);
    void wait(uint64_t ms);
    void printSettledBoard() const;
};