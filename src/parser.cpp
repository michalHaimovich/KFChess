#include "parser.hpp"
#include <sstream>
#include <stdexcept>
#include <algorithm>

// 1. Split a line into words (tokens) while ignoring whitespace
std::vector<std::string> Parser::splitLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// 2. Validation of a single board token
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

// 3. Converting a text token to a Square object
Board::Square Parser::parseToken(const std::string& token) {
    if (token == ".") {
        return { '.', '\0' };
    }
    return { token[0], token[1] };
}

// 4. Helper function: converting a text line to a structured command object
GameCommand Parser::parseCommandString(const std::string& line) {
    std::vector<std::string> tokens = splitLine(line);
    GameCommand cmd;
    cmd.type = CommandType::UNKNOWN;

    if (tokens.empty()) {
        return cmd;
    }

    // Parsing click command
    if (tokens[0] == "click") {
        if (tokens.size() == 3) {
            cmd.type = CommandType::CLICK;
            try {
                size_t pos1, pos2;
                cmd.arg1 = std::stoi(tokens[1], &pos1);
                cmd.arg2 = std::stoi(tokens[2], &pos2);

                if (pos1 != tokens[1].length() || pos2 != tokens[2].length()) {
                    throw std::invalid_argument("Garbage characters after number");
                }
            }
            catch (...) {
                cmd.type = CommandType::UNKNOWN;
            }
        }
    }
    else if (tokens[0] == "jump") {
        if (tokens.size() == 3) {
            cmd.type = CommandType::JUMP;
            try {
                cmd.arg1 = std::stoi(tokens[1]); // x
                cmd.arg2 = std::stoi(tokens[2]); // y
            }
            catch (...) {
                cmd.type = CommandType::UNKNOWN;
            }
        }
    }
    // Parsing wait command
    else if (tokens[0] == "wait") {
        if (tokens.size() == 2) {
            cmd.type = CommandType::WAIT;
            try {
                cmd.arg1 = std::stoi(tokens[1]); // ms
            }
            catch (...) {
                cmd.type = CommandType::UNKNOWN;
            }
        }
    }
  // Parsing print board command (must contain exactly these two words)
    else if (tokens[0] == "print") {
     if (tokens.size() == 2 && tokens[1] == "board") {
        cmd.type = CommandType::PRINT_BOARD;
        }
    }

  return cmd;
}

// 5. Main state machine of the parser
ParsedInput Parser::parseStream(std::istream& input) {
    enum State { WAITING_FOR_BOARD, READING_BOARD, READING_COMMANDS };
    State currentState = WAITING_FOR_BOARD;

    std::vector<std::vector<Board::Square>> tempRows;
  std::vector<GameCommand> commands;
    size_t expectedWidth = 0;

    std::string line;

    while (std::getline(input, line)) {
        // Clean hidden Windows characters
  if (!line.empty() && line.back() == '\r') {
            line.pop_back();
    }

      // Splitting the line into tokens automatically strips all whitespace!
        std::vector<std::string> tokens = splitLine(line);
    if (tokens.empty()) continue; // Skip empty lines or lines with only whitespace

     switch (currentState) {
     case WAITING_FOR_BOARD:
          // Now we check only the first word, without whitespace
        if (tokens[0] == "Board:") {
    currentState = READING_BOARD;
     }
         break;

 case READING_BOARD:
      if (tokens[0] == "Commands:") {
    currentState = READING_COMMANDS;
break;
   }

            // Board validation
   if (tempRows.empty()) {
 expectedWidth = tokens.size();
       }
         else if (tokens.size() != expectedWidth) {
throw std::runtime_error("ERROR ROW_WIDTH_MISMATCH");
     }

     {
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