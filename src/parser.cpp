#include "parser.hpp"
#include <sstream>
#include <stdexcept>

// 1. חלוקת שורה למילים (טוקנים) תוך התעלמות מרווחים
std::vector<std::string> Parser::splitLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    // מפעיל החילוץ (>>) מתעלם מרווחים אוטומטית
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// 2. ולידציה של טוקן בודד לפי דרישות המשחק
bool Parser::isValidToken(const std::string& token) {
    if (token == ".") {
        return true;
    }

    if (token.length() == 2) {
        char color = token[0];
        char type = token[1];

        // צבע חייב להיות שחור או לבן
        bool validColor = (color == 'w' || color == 'b');
        // סוג כלי חייב להיות אחד מהכלים המוכרים בשחמט (K, Q, R, B, N, P)
        bool validType = (type == 'K' || type == 'Q' || type == 'R' ||
            type == 'B' || type == 'N' || type == 'P');

        return validColor && validType;
    }

    return false;
}

// 3. המרת טוקן טקסטואלי לאובייקט Square
Board::Square Parser::parseToken(const std::string& token) {
    if (token == ".") {
        return { '.', '\0' };
    }
    return { token[0], token[1] };
}

// 4. מכונת המצבים הראשית
ParsedInput Parser::parseStream(std::istream& input) {
    // מצבי המכונה
    enum State { WAITING_FOR_BOARD, READING_BOARD, READING_COMMANDS };
    State currentState = WAITING_FOR_BOARD;

    // משתני עזר לשמירת הנתונים תוך כדי קריאה
    std::vector<std::vector<Board::Square>> tempRows;
    std::vector<std::string> commands;
    size_t expectedWidth = 0;

    std::string line;

    // קריאת הקלט שורה אחר שורה
    while (std::getline(input, line)) {

        // תיקון קריטי לסביבות מעורבות: הסרת תו '\r' ש-Windows מוסיף לסוף שורות
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // התעלמות משורות ריקות לחלוטין (אלא אם זה חלק מפקודה)
        if (line.empty()) continue;

        switch (currentState) {
        case WAITING_FOR_BOARD:
            if (line == "Board:") {
                currentState = READING_BOARD;
            }
            break;

        case READING_BOARD:
            if (line == "Commands:") {
                currentState = READING_COMMANDS;
                break;
            }

            {
                std::vector<std::string> tokens = splitLine(line);

                if (tokens.empty()) break;

                // בדיקת חריגת רוחב (VPL Requirement)
                if (tempRows.empty()) {
                    expectedWidth = tokens.size();
                }
                else if (tokens.size() != expectedWidth) {
                    throw std::runtime_error("ERROR ROW_WIDTH_MISMATCH");
                }

                std::vector<Board::Square> currentRow;
                for (const std::string& token : tokens) {
                    // בדיקת טוקן חוקי (VPL Requirement)
                    if (!isValidToken(token)) {
                        throw std::runtime_error("ERROR UNKNOWN_TOKEN");
                    }
                    currentRow.push_back(parseToken(token));
                }
                tempRows.push_back(currentRow);
            }
            break;

        case READING_COMMANDS:
            commands.push_back(line);
            break;
        }
    }

    // סיימנו לקרוא. כעת נארוז את המערך הזמני לתוך אובייקט Board האמיתי.
    size_t height = tempRows.size();
    Board board(expectedWidth, height);

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < expectedWidth; ++x) {
            board.place(x, y, tempRows[y][x]);
        }
    }

    // מחזירים את האובייקט המלא (מאתחלים אותו ישירות כאן כי ללוח אין בנאי ריק)
    return ParsedInput{ board, commands };
}