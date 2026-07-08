#include "pieces.hpp"
#include <cmath>
#include <algorithm>

// --- פונקציית עזר: בדיקה שהמסלול פנוי (לכלים שמחליקים על הלוח) ---
static bool isPathClear(int startX, int startY, int destX, int destY, const Board& board) {
    int dx = destX - startX;
    int dy = destY - startY;

    // קביעת כיוון ההתקדמות (1, -1, או 0)
    int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

    int currentX = startX + stepX;
    int currentY = startY + stepY;

    // סורקים את המשבצות עד שמגיעים ליעד
    while (currentX != destX || currentY != destY) {
        if (board.at(currentX, currentY).type != '\0') {
            return false; // הדרך חסומה על ידי כלי אחר
        }
        currentX += stepX;
        currentY += stepY;
    }
    return true;
}

// --- מימוש חוקי התנועה (Geometry) ---

bool King::isValidMove(int startX, int startY, int destX, int destY, const Board& board) const {
    int dx = std::abs(destX - startX);
    int dy = std::abs(destY - startY);
    // המלך זז בדיוק משבצת אחת לכל כיוון
    return (dx <= 1 && dy <= 1);
}

bool Rook::isValidMove(int startX, int startY, int destX, int destY, const Board& board) const {
    int dx = std::abs(destX - startX);
    int dy = std::abs(destY - startY);
    // תנועה רק על ציר אחד
    if (dx != 0 && dy != 0) return false;
    return isPathClear(startX, startY, destX, destY, board);
}

bool Bishop::isValidMove(int startX, int startY, int destX, int destY, const Board& board) const {
    int dx = std::abs(destX - startX);
    int dy = std::abs(destY - startY);
    // תנועה אלכסונית מושלמת (שינוי ב-X שווה לשינוי ב-Y)
    if (dx != dy) return false;
    return isPathClear(startX, startY, destX, destY, board);
}

bool Queen::isValidMove(int startX, int startY, int destX, int destY, const Board& board) const {
    int dx = std::abs(destX - startX);
    int dy = std::abs(destY - startY);
    // הלוגיקה היא שילוב של צריח (אחד מהם 0) או רץ (שווים)
    if ((dx == 0 || dy == 0) || (dx == dy)) {
        return isPathClear(startX, startY, destX, destY, board);
    }
    return false;
}

bool Knight::isValidMove(int startX, int startY, int destX, int destY, const Board& board) const {
    int dx = std::abs(destX - startX);
    int dy = std::abs(destY - startY);
    // תנועת סוס: 2 צעדים בכיוון אחד, ו-1 בכיוון השני (צורת L)
    // הסוס יכול לקפוץ מעל כלים, ולכן אין צורך לבדוק isPathClear
    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

bool Pawn::isValidMove(int startX, int startY, int destX, int destY, const Board& board) const {
    char myColor = board.at(startX, startY).color;
    int forwardDirection = (myColor == 'w') ? -1 : 1;

    int dx = destX - startX;
    int dy = destY - startY;

    // 1. תנועה ישרה (ללא אכילה)
    if (dx == 0) {
        // מקרה א': צעד אחד קדימה
        if (dy == forwardDirection) {
            return board.at(destX, destY).type == '\0';
        }

        // מקרה ב': שני צעדים קדימה (רק מהשורה ההתחלתית)
        if (dy == 2 * forwardDirection) {
            bool isWhiteStartRow = (myColor == 'w' && startY >= board.getHeight() - 2);
            bool isBlackStartRow = (myColor == 'b' && startY <= 1);

            if (isWhiteStartRow || isBlackStartRow) {
                // חייבים לוודא שגם משבצת הביניים וגם היעד ריקות!
                bool isIntermediateClear = board.at(startX, startY + forwardDirection).type == '\0';
                bool isDestClear = board.at(destX, destY).type == '\0';

                return isIntermediateClear && isDestClear;
            }
        }
        return false;
    }

    // 2. תנועה אלכסונית (אכילה בלבד)
    if (std::abs(dx) == 1 && dy == forwardDirection) {
        return board.at(destX, destY).type != '\0';
    }

    return false;
}

void Pawn::onLanding(Board& board, int destX, int destY, char color) const {
    // החייל בודק אם הוא הגיע לסוף הלוח, ומכתיר את עצמו ישירות על הלוח
    bool whiteReachedEnd = (color == 'w' && destY == 0);
    bool blackReachedEnd = (color == 'b' && destY == board.getHeight() - 1);

    if (whiteReachedEnd || blackReachedEnd) {
        board.place(destX, destY, { color, 'Q' });
    }
}

const Piece* PieceFactory::getPiece(char pieceType) {
    // מופעים סטטיים: נוצרים רק פעם אחת בזיכרון (Flyweight)
    static King king;
    static Rook rook;
    static Bishop bishop;
    static Queen queen;
    static Knight knight;
    static Pawn pawn;

    switch (std::toupper(pieceType)) {
    case 'K': return &king;
    case 'R': return &rook;
    case 'B': return &bishop;
    case 'Q': return &queen;
    case 'N': return &knight;
    case 'P': return &pawn;
    default: return nullptr;
    }
}