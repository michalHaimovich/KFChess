#include "game_engine.hpp"

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
    // 1. תרגום מפיקסלים למשבצות מטריצה (חלוקה בשלמים)
    int targetX = pixelX / 100;
    int targetY = pixelY / 100;

    // 2. תנאי שומר (Guard Clause) - התעלמות מקליק מחוץ ללוח (Test 3)
    if (targetX < 0 || targetX >= board.getWidth() ||
        targetY < 0 || targetY >= board.getHeight()) {
        return;
    }

    Board::Square targetSquare = board.at(targetX, targetY);

    // 3. מכונת המצבים של הבחירה
    if (!selectedSquare.has_value()) {
        // --- מצב א': אין כלי בחור ---

        if (targetSquare.type != '\0') {
            // קליק על כלי חוקי מעביר אותנו למצב "בחור" (Test 1)
            selectedSquare = { targetX, targetY };
        }
        // אם המשבצת ריקה, מתעלמים ולא עושים כלום (Test 2)

    }
    else {
        // --- מצב ב': כבר יש כלי בחור מקודם ---

        int startX = selectedSquare->first;
        int startY = selectedSquare->second;
        Board::Square selectedPiece = board.at(startX, startY);

        if (targetSquare.type != '\0' && isSameColor(selectedPiece, targetSquare)) {
            // קליק על כלי מאותו צבע - מחליף את הבחירה אליו (Test 4)
            selectedSquare = { targetX, targetY };
        }
        else {
            // קליק על משבצת ריקה או אויב - ביצוע תנועה (Move Request)

            // מבצעים את התנועה בזיכרון של הלוח
            board.place(targetX, targetY, selectedPiece);
            // מנקים את משבצת המקור (מחזירים אותה להיות נקודה)
            board.place(startX, startY, { '.', '\0' });

            // מאפסים את הסטייט חזרה למצב שבו "אין בחירה"
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