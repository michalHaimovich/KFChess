#include <iostream>
#include <stdexcept>
#include "parser.hpp"
#include "board.hpp"

int main() {
    try {
        // 1. קריאה ופענוח של כל הקלט
        ParsedInput gameData = Parser::parseStream(std::cin);

        // הלוח חולץ בהצלחה ונמצא בתוך gameData.board
        Board currentBoard = gameData.board;

        // 2. לולאת הפקודות (Command Loop)
        for (const std::string& command : gameData.commands) {

            if (command == "print board") {
                // המטלה באיטרציה הראשונה
                std::cout << currentBoard.toCanonicalString();
            }
            else if (command == "move wK e4") {
                // דוגמה להמחשה בלבד לאיטרציות הבאות!
                // currentBoard.place(4, 4, ...); 
            }
            // אפשר להוסיף כאן עוד פקודות בקלות...
        }
    }
    // 3. טיפול בשגיאות מה-Parser (לפי דרישות ה-VPL)
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        return 1;
    }

    return 0;
}