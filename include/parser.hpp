#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "board.hpp"

// מגדיר את סוגי הפקודות שהמערכת שלנו מכירה

enum class CommandType {
    CLICK,
    WAIT,
    PRINT_BOARD,
    UNKNOWN,
    JUMP
};

// מבנה נתונים שמייצג פקודה מוכנה לפעולה (אחרי שחולצה מהטקסט)
struct GameCommand {
    CommandType type;
    int arg1 = 0; // ישמש כ-X בקליק, או כזמן ב-Wait
    int arg2 = 0; // ישמש כ-Y בקליק
};

// עכשיו ה-Parser מחזיר רשימה של אובייקטי פקודה, ולא סתם מחרוזות
struct ParsedInput {
    Board board;
    std::vector<GameCommand> commands;
};

class Parser {
public:
    static ParsedInput parseStream(std::istream& input);

private:
    static bool isValidToken(const std::string& token);
    static Board::Square parseToken(const std::string& token);
    static std::vector<std::string> splitLine(const std::string& line);

    // פונקציית עזר חדשה שממירה שורת טקסט לאובייקט GameCommand
    static GameCommand parseCommandString(const std::string& line);
};