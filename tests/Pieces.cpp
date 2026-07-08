#include "doctest.h"
#include "pieces.hpp"
#include "board.hpp"

// ==========================================
// 1. PieceFactory & Base Contract Tests
// ==========================================
TEST_CASE("PieceFactory and Base Piece Logic") {
    SUBCASE("Factory generates correct valid pieces") {
        const Piece* king = PieceFactory::getPiece('K');
        REQUIRE(king != nullptr);
        CHECK(king->isVital() == true); // המלך חייב להיות חיוני

        const Piece* rook = PieceFactory::getPiece('R');
        REQUIRE(rook != nullptr);
        CHECK(rook->isVital() == false); // צריח אינו חיוני
    }

    SUBCASE("Factory handles invalid pieces gracefully") {
        // בהנחה שהפקטורי שלך מחזיר nullptr עבור כלי לא חוקי
        const Piece* invalid = PieceFactory::getPiece('X');
        CHECK(invalid == nullptr);
    }
}

// ==========================================
// 2. King Movement Tests
// ==========================================
TEST_CASE("King - Movement Geometry") {
    Board b(8, 8);
    const Piece* king = PieceFactory::getPiece('K');
    REQUIRE(king != nullptr);

    SUBCASE("Valid 1-step moves in all directions") {
        // המלך עומד ב-(4,4)
        CHECK(king->isValidMove(4, 4, 4, 3, b) == true); // למעלה
        CHECK(king->isValidMove(4, 4, 4, 5, b) == true); // למטה
        CHECK(king->isValidMove(4, 4, 3, 4, b) == true); // שמאלה
        CHECK(king->isValidMove(4, 4, 5, 4, b) == true); // ימינה
        CHECK(king->isValidMove(4, 4, 5, 5, b) == true); // אלכסון ימני למטה
        CHECK(king->isValidMove(4, 4, 3, 3, b) == true); // אלכסון שמאלי למעלה
    }

    SUBCASE("Invalid moves (too far)") {
        CHECK(king->isValidMove(4, 4, 4, 2, b) == false); // 2 צעדים ישר
        CHECK(king->isValidMove(4, 4, 6, 6, b) == false); // 2 צעדים באלכסון
        CHECK(king->isValidMove(4, 4, 4, 4, b) == false); // חוסר תזוזה
    }
}