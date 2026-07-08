#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "board.hpp"

// Defines the types of commands that the system recognizes
enum class CommandType {
    CLICK,
    WAIT,
    PRINT_BOARD,
    UNKNOWN,
    JUMP
};

// Data structure representing a command ready for action (after extracted from text)
struct GameCommand {
    CommandType type;
    int arg1 = 0; // Will serve as X in click, or time in wait
    int arg2 = 0; // Will serve as Y in click
};

// The parser now returns a list of command objects, not just strings
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

    // Helper function that converts a text line to a GameCommand object
  static GameCommand parseCommandString(const std::string& line);
};