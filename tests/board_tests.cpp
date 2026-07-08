#include "doctest.h"
#include "board.hpp"


TEST_CASE("Board - Initialization with extreme and normal dimensions") {
    SUBCASE("Normal rectangular board (non-square to test logic separation)") {
        Board b(5, 3);
        CHECK(b.getWidth() == 5);
        CHECK(b.getHeight() == 3);
        CHECK(b.at(4, 2).type == '\0');
    }

    SUBCASE("Zero dimensions board (Edge Case)") {
        Board b(0, 0);
        CHECK(b.getWidth() == 0);
        CHECK(b.getHeight() == 0);
        CHECK_THROWS_AS(b.at(0, 0), std::out_of_range);
    }

    SUBCASE("1x1 board (Minimum valid size)") {
        Board b(1, 1);
        CHECK(b.getWidth() == 1);
        CHECK(b.getHeight() == 1);
        CHECK(b.at(0, 0).type == '\0');
        CHECK_THROWS_AS(b.at(1, 0), std::out_of_range);
    }
}


TEST_CASE("Board - Piece Placement, Retrieval, and Overwriting") {
    Board b(8, 8);

    SUBCASE("Placing exactly on boundaries") {
        // Top-left corner
        b.place(0, 0, 'w', 'R');
        CHECK(b.at(0, 0).toString() == "wR");

    // Bottom-right corner (maximum boundary)
        b.place(7, 7, 'b', 'K');
        CHECK(b.at(7, 7).color == 'b');
        CHECK(b.at(7, 7).type == 'K');
    }

    SUBCASE("Overwriting existing pieces (Memory replacement)") {
        // Place a pawn
        b.place(4, 4, 'w', 'P');
        CHECK(b.at(4, 4).type == 'P');

        // Crush it with a queen (for example, in a promotion state)
      b.place(4, 4, 'w', 'Q');
  CHECK(b.at(4, 4).type == 'Q');

     // Delete it (place empty square)
        b.place(4, 4, '.', '\0');
        CHECK(b.at(4, 4).type == '\0');
        CHECK(b.at(4, 4).toString() == ".");
    }
}


TEST_CASE("Board - Boundary Violations and Exception Handling") {
 Board b(4, 4);

    SUBCASE("Accessing exactly 1 index out of bounds") {
      CHECK_THROWS_AS(b.at(4, 0), std::out_of_range); // X exceeds
        CHECK_THROWS_AS(b.at(0, 4), std::out_of_range); // Y exceeds
        CHECK_THROWS_AS(b.at(4, 4), std::out_of_range); // Both exceed
    }

    SUBCASE("Placing pieces out of bounds") {
        CHECK_THROWS_AS(b.place(5, 5, 'w', 'P'), std::out_of_range);

      Board::Square sq = { 'b', 'Q' };
      CHECK_THROWS_AS(b.place(100, 100, sq), std::out_of_range);
    }

    SUBCASE("Memory Underflow (Passing negative numbers to size_t)") {
        // Since size_t is unsigned, passing a negative number like -1
    // will cause underflow and turn into a huge positive number (Max Int).
        // The getIndex function must catch this and not crash!
     CHECK_THROWS_AS(b.at(-1, 0), std::out_of_range);
        CHECK_THROWS_AS(b.at(0, -1), std::out_of_range);
    }
}

// ==========================================
// 4. String representation (Formatting)
// ==========================================
TEST_CASE("Board - String Formatting and Canonical Output") {
    SUBCASE("Square toString logic") {
        Board::Square empty = { '.', '\0' };
        CHECK(empty.toString() == ".");

        Board::Square whiteKnight = { 'w', 'N' };
      CHECK(whiteKnight.toString() == "wN");

      // Testing unexpected input in square (software should concatenate what is given)
        Board::Square weirdPiece = { 'x', 'Z' };
        CHECK(weirdPiece.toString() == "xZ");
    }

    SUBCASE("Full canonical string generation (Spacing and Newlines)") {
        Board b(3, 2);

        // First row
        b.place(0, 0, 'w', 'K');
        // Middle remains empty
  b.place(2, 0, 'b', 'R');

    // Second row
        // Start empty
        b.place(1, 1, 'b', 'P');
        b.place(2, 1, 'w', 'Q');

        std::string expected =
       "wK . bR\n"
         ". bP wQ\n";

     CHECK(b.toCanonicalString() == expected);
    }

    SUBCASE("Canonical string of an empty board") {
        Board b(2, 2);
        std::string expected =
            ". .\n"
         ". .\n";

        CHECK(b.toCanonicalString() == expected);
    }
}