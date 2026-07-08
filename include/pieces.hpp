#pragma once
#include "board.hpp"

// --- החוזה המורחב ---
class Piece {
public:
    virtual bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const = 0;

    // פונקציות גנריות חדשות עם התנהגות ברירת מחדל (כדי שלא נצטרך לממש אותן בכל כלי)

    // האם אכילת הכלי הזה מסיימת את המשחק?
    virtual bool isVital() const { return false; }

    // מה קורה כשהכלי נוחת ביעד (מאפשר הכתרה, פיצוץ, או כל חוק עתידי אחר)
    virtual void onLanding(Board& board, int destX, int destY, char color) const {}

    virtual ~Piece() = default;
};

// --- מימושי הכלים (נשארים, אבל משתמשים בחוזה החדש) ---

class King : public Piece {
public:
    bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const override;
    // המלך מודיע שהוא כלי חיוני!
    bool isVital() const override { return true; }
};

class Pawn : public Piece {
public:
    bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const override;
    // החייל דורס את פעולת הנחיתה כדי לבדוק הכתרה
    void onLanding(Board& board, int destX, int destY, char color) const override;
};

// שאר הכלים נשארים רגילים לחלוטין, ומקבלים אוטומטית false ל-isVital וכלום ל-onLanding
class Rook : public Piece {
public:
    bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const override;
};

class Bishop : public Piece {
public:
    bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const override;
};

class Queen : public Piece {
public:
    bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const override;
};

class Knight : public Piece {
public:
    bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const override;
};

class PieceFactory {
public:
    static const Piece* getPiece(char pieceType);
};