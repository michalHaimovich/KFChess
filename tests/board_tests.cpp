#include <gtest/gtest.h>
#include "board.hpp"


// טסט 1: אתחול ומידות
TEST(BoardTest, InitializationAndDimensions) {
    Board board(8, 8); // לוח שחמט סטנדרטי

    // פקודות EXPECT_EQ בודקות ששני הערכים שווים 
    EXPECT_EQ(board.getWidth(), 8);
    EXPECT_EQ(board.getHeight(), 8);
}

// טסט 2: בדיקת גישה בטוחה וזריקת שגיאות
TEST(BoardTest, OutOfBoundsThrowsException) {
    Board board(4, 4);

    // אנחנו מצפים שהפונקציה תזרוק שגיאת std::out_of_range אם נגלוש מהגבולות
    EXPECT_THROW(board.at(4, 4), std::out_of_range);
    EXPECT_THROW(board.place(5, 0, 'w', 'K'), std::out_of_range);
}

// טסט 3: הצבת כלים ובדיקת משבצת
TEST(BoardTest, PlaceAndRetrievePieces) {
    Board board(2, 2); // לוח קטן 2x2

    board.place(0, 0, 'w', 'K'); // מלך לבן בפינה השמאלית העליונה
    board.place(1, 1, 'b', 'Q'); // מלכה שחורה בפינה הימנית התחתונה

    // בדיקה שהכלים אכן שם
    Board::Square sq1 = board.at(0, 0);
    EXPECT_EQ(sq1.color, 'w');
    EXPECT_EQ(sq1.type, 'K');

    // בדיקה שמשבצת שלא נגענו בה נשארה ריקה
    Board::Square sq_empty = board.at(1, 0);
    EXPECT_EQ(sq_empty.color, '.');
}

// טסט 4: מחקה את דרישות ה-VPL שראינו!
TEST(BoardTest, CanonicalStringOutput) {
    Board board(4, 3); // 4 עמודות, 3 שורות כמו בטסט של ה-VPL

    // שורה ראשונה
    board.place(0, 0, 'w', 'K');
    board.place(3, 0, 'b', 'K');
    // שורה שניה נשארת ריקה
    // שורה שלישית
    board.place(0, 2, 'w', 'R');
    board.place(3, 2, 'b', 'R');

    std::string expected_output =
        "wK . . bK\n"
        ". . . .\n"
        "wR . . bR\n";

    EXPECT_EQ(board.toCanonicalString(), expected_output);
}