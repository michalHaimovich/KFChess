#include "parser.hpp"
#include <sstream>
#include <stdexcept>
#include <algorithm>

// 1. חלוקת שורה למילים (טוקנים) תוך התעלמות מרווחים
std::vector<std::string> Parser::splitLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// 2. ולידציה של טוקן בודד של הלוח
bool Parser::isValidToken(const std::string& token) {
    if (token == ".") {
        return true;
    }

    if (token.length() == 2) {
        char color = token[0];
        char type = token[1];

        bool validColor = (color == 'w' || color == 'b');
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

// 4. פונקציית העזר החדשה: המרת שורת טקסט לאובייקט פקודה מובנה
GameCommand Parser::parseCommandString(const std::string& line) {
    std::vector<std::string> tokens = splitLine(line);
    GameCommand cmd;
    cmd.type = CommandType::UNKNOWN;

    if (tokens.empty()) {
        return cmd;
    }

    // ניתוח פקודת click
    if (tokens[0] == "click") {
        if (tokens.size() >= 3) {
            cmd.type = CommandType::CLICK;
            try {
                // המרת מחרוזת למספר שלם (Integer)
                cmd.arg1 = std::stoi(tokens[1]); // x
                cmd.arg2 = std::stoi(tokens[2]); // y
            }
            catch (...) {
                // אם ההמרה נכשלה (למשל הטקסט לא היה מספר חוקי)
                cmd.type = CommandType::UNKNOWN;
            }
        }
    }
    // ניתוח פקודת wait
    else if (tokens[0] == "wait") {
        if (tokens.size() >= 2) {
            cmd.type = CommandType::WAIT;
            try {
                cmd.arg1 = std::stoi(tokens[1]); // ms
            }
            catch (...) {
                cmd.type = CommandType::UNKNOWN;
            }
        }
    }
    // ניתוח פקודת print board (צריכה להכיל בדיוק את שתי המילים האלו)
    else if (tokens[0] == "print") {
        if (tokens.size() >= 2 && tokens[1] == "board") {
            cmd.type = CommandType::PRINT_BOARD;
        }
    }

    return cmd;
}

// 5. מכונת המצבים הראשית של ה-Parser
ParsedInput Parser::parseStream(std::istream& input) {
    enum State { WAITING_FOR_BOARD, READING_BOARD, READING_COMMANDS };
    State currentState = WAITING_FOR_BOARD;

    std::vector<std::vector<Board::Square>> tempRows;
    std::vector<GameCommand> commands; // שונה למערך של GameCommand!
    size_t expectedWidth = 0;

    std::string line;

    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

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

                if (tempRows.empty()) {
                    expectedWidth = tokens.size();
                }
                else if (tokens.size() != expectedWidth) {
                    throw std::runtime_error("ERROR ROW_WIDTH_MISMATCH");
                }

                std::vector<Board::Square> currentRow;
                for (const std::string& token : tokens) {
                    if (!isValidToken(token)) {
                        throw std::runtime_error("ERROR UNKNOWN_TOKEN");
                    }
                    currentRow.push_back(parseToken(token));
                }
                tempRows.push_back(currentRow);
            }
            break;

        case READING_COMMANDS:
            // ה-Parser מפענח את הפקודה ישירות ושומר אובייקט נקי ומובנה
            commands.push_back(parseCommandString(line));
            break;
        }
    }

    size_t height = tempRows.size();
    Board board(expectedWidth, height);

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < expectedWidth; ++x) {
            board.place(x, y, tempRows[y][x]);
        }
    }

    return ParsedInput{ board, commands };
}