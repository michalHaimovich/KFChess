#include "game_engine.hpp"
#include "pieces.hpp"

// אתחול המנוע: הלוח מועתק, הזמן מאופס, והבחירה מתחילה כריקה (nullopt)
GameEngine::GameEngine(const Board& initialBoard)
    : board(initialBoard), currentTimeMs(0), selectedSquare(std::nullopt) {
}

// מימוש פונקציית העזר לזיהוי צבעים
bool GameEngine::isSameColor(Board::Square p1, Board::Square p2) const {
    // אם אחת המשבצות ריקה, אין להן צבע משותף
    if (p1.type == '\0' || p2.type == '\0') return false;
    return p1.color == p2.color;
}

void GameEngine::handleClick(int pixelX, int pixelY) {
    int targetX = pixelX / 100;
    int targetY = pixelY / 100;

    if (targetX < 0 || targetX >= board.getWidth() ||
        targetY < 0 || targetY >= board.getHeight()) {
        return;
    }

    Board::Square targetSquare = board.at(targetX, targetY);

    if (!selectedSquare.has_value()) {
        if (targetSquare.type != '\0') {
            selectedSquare = { targetX, targetY };
        }
    }
    else {
        int startX = selectedSquare->first;
        int startY = selectedSquare->second;
        Board::Square selectedPiece = board.at(startX, startY);

        if (targetSquare.type != '\0' && isSameColor(selectedPiece, targetSquare)) {
            selectedSquare = { targetX, targetY }; // החלפת בחירה
        }
        else {
            // הסטייט: ניסיון תנועה ליעד (ריק או אויב)

            // 1. מבקשים מהמפעל את מחלקת ה-OOP של הכלי הספציפי
            const Piece* pieceRule = PieceFactory::getPiece(selectedPiece.type);

            // 2. שואלים את המחלקה אם הפעולה הגיונית (פולימורפיזם בפעולה)
            if (pieceRule != nullptr &&
                pieceRule->isValidMove(startX, startY, targetX, targetY, board)) {

                // התנועה חוקית מתמטית! מתקנים את הזיכרון של הלוח
                board.place(targetX, targetY, selectedPiece);
                board.place(startX, startY, { '.', '\0' });
            }

            // לפי הדרישות: גם אם המהלך תקין וגם אם הוא לא חוקי,
            // המהלך מסתיים (הבחירה מתבטלת).
            selectedSquare = std::nullopt;
        }
    }
}

void GameEngine::wait(uint64_t ms) {
    // קידום שעון המשחק
    currentTimeMs += ms;

    // הערה ארכיטקטונית: באיטרציות הבאות, כשנוסיף זמני המתנה ייחודיים לכל כלי,
    // כאן תרוץ לולאה שתעבור על "תור התנועות הממתינות" ותמקם אותן בלוח.
    // מכיוון שכרגע התנועה מתבצעת מידית ב-handleClick, הפונקציה רק מקדמת את הזמן.
}

void GameEngine::printSettledBoard() const {
    // מעבר על המטריצה והדפסתה בדיוק בפורמט שה-VPL מצפה לו
    for (size_t y = 0; y < board.getHeight(); ++y) {
        for (size_t x = 0; x < board.getWidth(); ++x) {
            Board::Square sq = board.at(x, y);

            if (sq.type == '\0') {
                std::cout << ".";
            }
            else {
                std::cout << sq.color << sq.type;
            }

            // רווח בין כלים, אך לא בסוף השורה
            if (x < board.getWidth() - 1) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}