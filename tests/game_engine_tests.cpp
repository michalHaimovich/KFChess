#pragma once

#include <gtest/gtest.h>
#include "../include/game_engine.hpp"
#include "../include/board.hpp"
#include <sstream>
#include <iostream>

// Test Fixture - מחלקת עזר שמרכזת קוד משותף לכל הטסטים של המנוע
class GameEngineTest : public ::testing::Test {
protected:
    // פונקציה שתופסת את ההדפסה של הלוח כדי שנוכל לבדוק אותה
    std::string getEngineOutput(const GameEngine& engine) {
        std::stringstream buffer;
        // שמירת המצביע המקורי של ההדפסה
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        engine.printSettledBoard();

        // החזרת ההדפסה למצב הרגיל
        std::cout.rdbuf(old);
        return buffer.str();
    }
};

// Test 1: select_piece_by_center_click
TEST_F(GameEngineTest, SelectPieceByCenterClick) {
    // הכנת לוח 3x3 עם מלך לבן בפינה השמאלית העליונה
    Board board(3, 3);
    board.place(0, 0, { 'w', 'K' });
    GameEngine engine(board);

    // ביצוע הפקודות
    engine.handleClick(50, 50);   // בחירת המלך
    engine.handleClick(150, 150); // בקשת תנועה לאמצע הלוח
    engine.wait(1000);

    std::string expected =
        ". . .\n"
        ". wK .\n"
        ". . .\n";

    EXPECT_EQ(getEngineOutput(engine), expected);
}

// Test 2: click_empty_cell_does_not_select
TEST_F(GameEngineTest, ClickEmptyCellDoesNotSelect) {
    Board board(3, 3);
    board.place(0, 0, { 'w', 'K' });
    GameEngine engine(board);

    engine.handleClick(150, 150); // קליק על משבצת ריקה
    engine.handleClick(250, 250); // קליק על משבצת ריקה אחרת (ניסיון תנועה)
    engine.wait(1000);

    // הלוח אמור להישאר בדיוק אותו דבר
    std::string expected =
        "wK . .\n"
        ". . .\n"
        ". . .\n";

    EXPECT_EQ(getEngineOutput(engine), expected);
}

// Test 3: click_outside_board_is_ignored
TEST_F(GameEngineTest, ClickOutsideBoardIsIgnored) {
    Board board(3, 3);
    board.place(0, 0, { 'w', 'K' });
    GameEngine engine(board);

    engine.handleClick(350, 50);  // חריגה מגבולות (X > 300)
    engine.handleClick(-10, 50);  // חריגה מגבולות (X שלילי)

    std::string expected =
        "wK . .\n"
        ". . .\n"
        ". . .\n";

    EXPECT_EQ(getEngineOutput(engine), expected);
}

// Test 4: clicking_another_piece_replaces_selection
TEST_F(GameEngineTest, ClickingAnotherPieceReplacesSelection) {
    Board board(3, 3);
    board.place(0, 0, { 'w', 'R' });
    board.place(2, 0, { 'w', 'K' });
    GameEngine engine(board);

    engine.handleClick(50, 50);   // בחירת הצריח (0,0)
    engine.handleClick(250, 50);  // לחיצה על המלך מאותו צבע (2,0) - אמור להחליף בחירה
    engine.handleClick(250, 150); // הזזת הכלי הנבחר (המלך) למטה
    engine.wait(1000);

    std::string expected =
        "wR . .\n"
        ". . wK\n"
        ". . .\n";

    EXPECT_EQ(getEngineOutput(engine), expected);
}