#include <iostream>
#include <stdexcept>
#include "parser.hpp"
#include "game_engine.hpp"

int main() {
    try {
        // 1. קריאה ופענוח של כל הקלט (מהקונסול או מקובץ הבדיקה של ה-VPL)
        ParsedInput gameData = Parser::parseStream(std::cin);

        // 2. אתחול מנוע המשחק עם הלוח שהתקבל
        GameEngine engine(gameData.board);

        // 3. לולאת הפקודות (Command Loop)
        for (const GameCommand& cmd : gameData.commands) {

            // ניתוב הפקודה לפונקציה המתאימה במנוע
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

                // --- התוספת שלנו: טיפול בפקודת הקפיצה ---
            case CommandType::JUMP:
                // arg1 = x, arg2 = y
                engine.handleJump(cmd.arg1, cmd.arg2);
                break;
                // ----------------------------------------

            case CommandType::UNKNOWN:
                // פקודה משובשת או לא מוכרת - מתעלמים בבטחה
                break;
            }
        }
    }
    // 4. טיפול בשגיאות קריטיות של שלב האתחול (כמו חריגה ברוחב או טוקן לא חוקי)
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        // החזרת קוד שגיאה למערכת ההפעלה
        return 1;
    }

    return 0; // סיום תקין
}