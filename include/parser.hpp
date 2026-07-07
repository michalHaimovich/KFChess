#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "board.hpp"

// המבנה שמאגד את כל מה שה-Parser הבין מהקלט
struct ParsedInput {
    Board board;
    std::vector<std::string> commands;
};

class Parser {
public:
    // עכשיו הפונקציה הסטטית מחזירה את כל חבילת המידע
    static ParsedInput parseStream(std::istream& input);

private:
    static bool isValidToken(const std::string& token);
    static Board::Square parseToken(const std::string& token);
    static std::vector<std::string> splitLine(const std::string& line);
};