#include "game_engine.hpp"
#include "pieces.hpp"
#include <cmath>


GameEngine::GameEngine(const Board& initialBoard, const EngineConfig& cfg)
    : board(initialBoard), config(cfg), currentTimeMs(0), selectedSquare(std::nullopt) {
    lockedSquares.resize(board.getHeight(), std::vector<bool>(board.getWidth(), false));
    jumpExpiration.resize(board.getHeight(), std::vector<uint64_t>(board.getWidth(), 0));
}

bool GameEngine::isSameColor(Board::Square p1, Board::Square p2) const {
    if (p1.type == '\0' || p2.type == '\0') return false;
    return p1.color == p2.color;
}

bool GameEngine::isSquareLocked(int x, int y) const {
    if (x < 0 || x >= board.getWidth() || y < 0 || y >= board.getHeight()) return false;
    return lockedSquares[y][x];
}

uint64_t GameEngine::calculateArrival(int startX, int startY, int destX, int destY) const {
    int dx = std::abs(destX - startX);
    int dy = std::abs(destY - startY);
    int distance = std::max(dx, dy);

    return currentTimeMs + static_cast<uint64_t>(distance * config.moveMsPerCell);
}

void GameEngine::handleClick(int pixelX, int pixelY) {

    if (isGameOver) {
        return;
    }
    int targetX = pixelX / config.squareSizePixels;
    int targetY = pixelY / config.squareSizePixels;

    if (targetX < 0 || targetX >= board.getWidth() || targetY < 0 || targetY >= board.getHeight()) {
        return;
    }

    if (isSquareLocked(targetX, targetY)) return;

    Board::Square targetSquare = board.at(targetX, targetY);

    if (!selectedSquare.has_value()) {
        if (targetSquare.type != '\0') {
            selectedSquare = { targetX, targetY };
        }
    }
    else {
        int startX = selectedSquare->first;
        int startY = selectedSquare->second;

        if (isSquareLocked(startX, startY)) {
            selectedSquare = std::nullopt;
            return;
        }
        Board::Square selectedPiece = board.at(startX, startY);

        // הגנה: אי אפשר להתחיל תנועה חדשה אם הכלי כרגע קופץ באוויר!
        if (jumpExpiration[startY][startX] > currentTimeMs) {
            selectedSquare = std::nullopt;
            return;
        }

        if (targetSquare.type != '\0' && isSameColor(selectedPiece, targetSquare)) {
            selectedSquare = { targetX, targetY };
        }
        else {
            const Piece* pieceRule = PieceFactory::getPiece(selectedPiece.type);

            if (pieceRule != nullptr &&
                pieceRule->isValidMove(startX, startY, targetX, targetY, board)) {

                std::vector<std::pair<int, int>> movePath;
                // ... (קוד חישוב המסלול שלך נשאר זהה) ...

                bool pathBlocked = false;
                for (const auto& p : movePath) {
                    // --- שינוי קריטי: היעד אינו נבדק כחסום ואינו ננעל! ---
                    // זה מאפשר לשני כלים לנסות לנחות באותו מקום, או לכלי המותקף להגן על עצמו
                    if (p.first != targetX || p.second != targetY) {
                        if (isSquareLocked(p.first, p.second)) {
                            pathBlocked = true;
                            break;
                        }
                    }
                }

                if (!pathBlocked) {
                    for (const auto& p : movePath) {
                        // נועלים רק את הדרך, לא את היעד הסופי!
                        if (p.first != targetX || p.second != targetY) {
                            lockedSquares[p.second][p.first] = true;
                        }
                    }  

                    pendingQueue.push({
                        startX, startY, targetX, targetY,
                        calculateArrival(startX, startY, targetX, targetY),
                        selectedPiece,
                        movePath // שומרים את המסלול כדי לדעת מה לשחרר ב-wait
                    });
                }
            }

            // איפוס הבחירה בסוף התהליך
            selectedSquare = std::nullopt;
        }
    }
}

void GameEngine::handleJump(int pixelX, int pixelY) {
    if (isGameOver) return;

    int targetX = pixelX / config.squareSizePixels;
    int targetY = pixelY / config.squareSizePixels;

    if (targetX < 0 || targetX >= board.getWidth() || targetY < 0 || targetY >= board.getHeight()) {
        return;
    }

    Board::Square piece = board.at(targetX, targetY);

    // אי אפשר לקפוץ אם אין שם כלי, או אם המשבצת נעולה (הכלי יצא לדרך)
    if (piece.type == '\0' || isSquareLocked(targetX, targetY)) {
        return;
    }

    // אם הכלי לא קופץ כרגע, נפעיל לו קפיצה ל-1000 מילישניות
    if (jumpExpiration[targetY][targetX] <= currentTimeMs) {
        jumpExpiration[targetY][targetX] = currentTimeMs + config.jumpDurationMs;
    }

    // אם הכלי שקפץ היה מסומן, נבטל את הסימון (כמו ב-click)
    selectedSquare = std::nullopt;
}

void GameEngine::wait(uint64_t ms) {
    currentTimeMs += ms;

    while (!pendingQueue.empty() && pendingQueue.top().arrivalTime <= currentTimeMs) {
        PendingMove move = pendingQueue.top();
        pendingQueue.pop();

        // נבדוק מה יש ביעד
        Board::Square targetSquare = board.at(move.destX, move.destY);
        bool targetIsEnemy = (targetSquare.type != '\0' && targetSquare.color != move.piece.color);

        if (targetIsEnemy && jumpExpiration[move.destY][move.destX] >= move.arrivalTime) {
            // התוקף נפל למלכודת! הכלי שבאוויר אוכל אותו.

            // אם התוקף שהושמד הוא במקרה המלך (יצא להתקפה ומת), המשחק נגמר!
            if (PieceFactory::getPiece(move.piece.type)->isVital()) {
                isGameOver = true;
            }

            // מוחקים את התוקף ממשבצת המקור שלו (הוא לא נוחת ביעד)
            board.place(move.startX, move.startY, { '.', '\0' });
        }
        else {
            // נחיתה רגילה (אין קפיצה, או שהקפיצה הסתיימה)

            // אחרי שהנחתנו את הכלי על הלוח ב-wait:
            board.place(move.destX, move.destY, move.piece);
            board.place(move.startX, move.startY, { '.', '\0' });

            // אומרים לכלי: "נחתת! תעשה מה שאתה אמור לעשות"
            PieceFactory::getPiece(move.piece.type)->onLanding(board, move.destX, move.destY, move.piece.color);

            // בדיקת Game Over (אם דרסנו מלך נייח)

            if (targetSquare.type != '\0' && PieceFactory::getPiece(targetSquare.type)->isVital()) {
                isGameOver = true;
            }

        }

        // שחרור הנעילות לאורך המסלול (נשאר זהה)
        for (const auto& p : move.path) {
            if (p.first != move.destX || p.second != move.destY) {
                lockedSquares[p.second][p.first] = false;
            }
        }
    }
}
void GameEngine::printSettledBoard() const {
    for (size_t y = 0; y < board.getHeight(); ++y) {
        for (size_t x = 0; x < board.getWidth(); ++x) {
            Board::Square sq = board.at(x, y);

            if (sq.type == '\0') {
                std::cout << ".";
            }
            else {
                std::cout << sq.color << sq.type;
            }

            if (x < board.getWidth() - 1) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}