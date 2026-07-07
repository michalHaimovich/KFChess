#pragma once // מונע ייבוא כפול של הקובץ בתהליך הקימפול

#include <vector>
#include <string>
#include <stdexcept> // בשביל חריגות כמו std::out_of_range

class Board {
public:
    // הגדרת המבנה הפנימי של משבצת
    struct Square {
        char color; // 'w', 'b', or '.'
        char type;  // 'K', 'Q', 'R', etc.

        // פונקציית עזר להמרת המשבצת למחרוזת (לפי דרישות ה-VPL)
        std::string toString() const {
            if (color == '.') return ".";
            return std::string(1, color) + std::string(1, type);
        }
    };

    // בנאי: מקבל מידות ומאתחל לוח ריק
    Board(size_t width, size_t height);

    // פונקציות גישה (Getters)
    size_t getWidth() const;
    size_t getHeight() const;

    // קריאת משבצת ממיקום ספציפי (בטוחה מפני חריגת גבולות)
    Square at(size_t x, size_t y) const;

    // הצבת משבצת/כלי במיקום ספציפי
    void place(size_t x, size_t y, char color, char type);

    // פונקציה זו מקבלת משבצת מוכנה במקום פרמטרים נפרדים
    void place(size_t x, size_t y, const Square& square);

    // מחזירה את כל הלוח כמחרוזת אחת בדיוק בפורמט הנדרש
    std::string toCanonicalString() const;

private:
    size_t width_;
    size_t height_;

    // המיכל בפועל: מערך חד-ממדי רציף בזיכרון ליעילות מטמון מקסימלית
    std::vector<Square> grid_;

    // פונקציית עזר פנימית לחישוב אינדקס
    size_t getIndex(size_t x, size_t y) const;
};