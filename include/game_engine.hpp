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
    int squareSizePixels = 100;       // Square size for converting clicks
    uint64_t moveMsPerCell = 500;     // Movement time per cell
    uint64_t jumpDurationMs = 1000;   // Air time duration in jump
};

// Updated structure to save the full path
struct PendingMove {
    int startX, startY;
    int destX, destY;
    uint64_t arrivalTime;
    Board::Square piece;

    // Stores all squares the piece traveled through
    std::vector<std::pair<int, int>> path;

    bool operator<(const PendingMove& other) const {
        return arrivalTime > other.arrivalTime;
    }
};

class GameEngine {
private:
    Board board;
    EngineConfig config;        // Store configuration in the engine

    uint64_t currentTimeMs;
    std::optional<std::pair<int, int>> selectedSquare;

    std::priority_queue<PendingMove> pendingQueue;
    std::vector<std::vector<bool>> lockedSquares;
    std::vector<std::vector<uint64_t>> jumpExpiration;

    bool isGameOver = false;

    bool isSameColor(Board::Square p1, Board::Square p2) const;
    bool isSquareLocked(int x, int y) const;

    // Function moved inside so it can read config.moveMsPerCell
    uint64_t calculateArrival(int startX, int startY, int destX, int destY) const;

public:
    explicit GameEngine(const Board& initialBoard, const EngineConfig& cfg = EngineConfig());

    void handleClick(int pixelX, int pixelY);
    void handleJump(int pixelX, int pixelY);
    void wait(uint64_t ms);
    void printSettledBoard() const;
};