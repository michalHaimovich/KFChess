#include <iostream>
#include <stdexcept>
#include "parser.hpp"
#include "game_engine.hpp"


// https://github.com/michalHaimovich/KFChess.git

int main() {
    try {
        // 1. Read and parse all input (from console or from VPL test file)
        ParsedInput gameData = Parser::parseStream(std::cin);

        // 2. Initialize the game engine with the received board
        GameEngine engine(gameData.board);

        // 3. Command loop
        for (const GameCommand& cmd : gameData.commands) {

            // Route the command to the appropriate function in the engine
            switch (cmd.type) {
            case CommandType::CLICK:
                // arg1 = x, arg2 = y
                engine.handleClick(cmd.arg1, cmd.arg2);
                break;

            case CommandType::WAIT:
                // arg1 = ms
                engine.wait(cmd.arg1);
                break;

            case CommandType::PRINT_BOARD:
                engine.printSettledBoard();
                break;

                // --- Our addition: handling the jump command ---
            case CommandType::JUMP:
                // arg1 = x, arg2 = y
                engine.handleJump(cmd.arg1, cmd.arg2);
                break;
                // ----------------------------------------

            case CommandType::UNKNOWN:
                // Broken or unknown command - ignore safely
                break;
            }
        }
    }
    // 4. Handle critical errors from initialization phase (e.g., width violation or invalid token)
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        // Return error code to operating system
        return 1;
    }

    return 0; // Normal completion
}