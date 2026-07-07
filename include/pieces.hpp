#pragma once

#include "board.hpp"
#include <cstdint>

// מחלקת האב (Interface)
class Piece {
public:
    virtual ~Piece() = default;

    // פונקציה וירטואלית טהורה: בודקת אם הגיאומטריה של המהלך חוקית
    virtual bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const = 0;

    // תשתית לעתיד (כפי שביקשת): חישוב זמן המתנה פר-כלי
    virtual uint64_t calculateMoveTime(int distance) const { return 0; }
};

// המחלקות היורשות (כל אחת מממשת את החוקים שלה)
class King : public Piece {
public:
    bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const override;
};

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
class Pawn : public Piece {
public:
    bool isValidMove(int startX, int startY, int destX, int destY, const Board& board) const override;
};

// ה-Factory: מייצר ומספק לנו את מחלקת החוקים המתאימה לפי תו
class PieceFactory {
public:
    // מחזיר מצביע (Pointer) לאובייקט הסטטי המתאים, או nullptr אם הכלי לא מוכר
    static const Piece* getPiece(char pieceType);
};