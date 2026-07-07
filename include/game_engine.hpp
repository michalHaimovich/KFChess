#pragma once

#include <optional>
#include <utility>
#include <iostream>
#include "board.hpp"

class GameEngine {
private:
    Board board;                  // מופע הלוח עצמו
    uint64_t currentTimeMs;       // שעון המשחק במילישניות

    // משתנה הסטייט של הבחירה: שומר זוג קואורדינטות (x,y) אם יש כלי נבחר
    std::optional<std::pair<int, int>> selectedSquare;

    // פונקציית עזר פרטית: בודקת האם שני כלים הם בעלי אותו צבע (Friendly pieces)
    bool isSameColor(Board::Square p1, Board::Square p2) const;

public:
    // בנאי שמקבל את הלוח ההתחלתי שאותחל על ידי ה-Parser
    explicit GameEngine(const Board& initialBoard);

    // ממשק הפקודות החיצוני (Controller Interface)
    void handleClick(int pixelX, int pixelY);
    void wait(uint64_t ms);

    // הפונקציה קבועה (const) כי הדפסה לא משנה את מצב המערכת
    void printSettledBoard() const;
};