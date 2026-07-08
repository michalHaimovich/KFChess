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
    }
}
// ==========================================
// 3. Pawn Movement & Capture Tests
// ==========================================
TEST_CASE("Pawn - Directional Movement and Captures") {
    Board b(8, 8);
    const Piece* pawn = PieceFactory::getPiece('P');

    // מציבים חייל לבן בשורת ההתחלה שלו (y=6, מתקדם ל-y=5)
    b.place(4, 6, 'w', 'P');
    // מציבים חייל שחור בשורת ההתחלה שלו (y=1, מתקדם ל-y=2)
    b.place(4, 1, 'b', 'P');

    SUBCASE("White Pawn - Valid forward movements") {
        CHECK(pawn->isValidMove(4, 6, 4, 5, b) == true); // צעד אחד למעלה
        CHECK(pawn->isValidMove(4, 6, 4, 4, b) == true); // צעד כפול בהתחלה
    }

    SUBCASE("Black Pawn - Valid forward movements") {
        CHECK(pawn->isValidMove(4, 1, 4, 2, b) == true); // צעד אחד למטה
        CHECK(pawn->isValidMove(4, 1, 4, 3, b) == true); // צעד כפול בהתחלה
    }

    SUBCASE("Pawns cannot move forward if blocked") {
        // שמים כלי חוסם (לא משנה הצבע, GameEngine מטפל בזה, החייל פשוט לא יכול להתקדם)
        b.place(4, 5, 'b', 'R');
        CHECK(pawn->isValidMove(4, 6, 4, 5, b) == false); // חסום בצעד אחד
        CHECK(pawn->isValidMove(4, 6, 4, 4, b) == false); // חסום בצעד כפול למרות שהיעד ריק (כי הדרך חסומה)
    }

    SUBCASE("Pawn Diagonal Captures") {
        // מנסים לזוז באלכסון למשבצת ריקה - אמור להיות לא חוקי
        CHECK(pawn->isValidMove(4, 6, 5, 5, b) == false);

        // עכשיו נשים כלי באלכסון
        b.place(5, 5, 'b', 'R'); // כלי שחור באלכסון ללבן
        b.place(3, 2, 'w', 'R'); // כלי לבן באלכסון לשחור

        // עכשיו האלכסון חייב להיות חוקי כי יש מטרה
        CHECK(pawn->isValidMove(4, 6, 5, 5, b) == true); // לבן אוכל למעלה-ימינה
        CHECK(pawn->isValidMove(4, 1, 3, 2, b) == true); // שחור אוכל למטה-שמאלה
    }

    SUBCASE("Pawns cannot move backwards") {
        // נזיז את החייל הלבן לאמצע הלוח
        b.place(4, 4, 'w', 'P');
        CHECK(pawn->isValidMove(4, 4, 4, 5, b) == false); // לבן לא יכול לרדת
        CHECK(pawn->isValidMove(4, 4, 4, 6, b) == false);
    }
}

// ==========================================
// 4. Sliding Pieces (Rook, Bishop, Queen) - Raycasting Tests
// ==========================================
TEST_CASE("Sliding Pieces - Geometry and Obstacles") {
    Board b(8, 8);
    const Piece* rook = PieceFactory::getPiece('R');
    const Piece* bishop = PieceFactory::getPiece('B');
    const Piece* queen = PieceFactory::getPiece('Q');

    SUBCASE("Rook - Straight lines and blocks") {
        b.place(0, 0, 'w', 'R');

        // יעד חוקי ופנוי
        CHECK(rook->isValidMove(0, 0, 0, 5, b) == true); // אנכי
        CHECK(rook->isValidMove(0, 0, 5, 0, b) == true); // אופקי
        CHECK(rook->isValidMove(0, 0, 5, 5, b) == false); // אלכסון (לא חוקי לצריח)

        // שמים מכשול בדרך
        b.place(0, 3, 'w', 'P');
        CHECK(rook->isValidMove(0, 0, 0, 5, b) == false); // הדרך חסומה!
        CHECK(rook->isValidMove(0, 0, 0, 2, b) == true);  // תזוזה עד המכשול חוקית
    }

    SUBCASE("Bishop - Diagonals and blocks") {
        b.place(4, 4, 'w', 'B');

        // יעד חוקי ופנוי
        CHECK(bishop->isValidMove(4, 4, 7, 7, b) == true); // אלכסון
        CHECK(bishop->isValidMove(4, 4, 1, 1, b) == true); // אלכסון שני
        CHECK(bishop->isValidMove(4, 4, 4, 7, b) == false); // ישר (לא חוקי לרץ)

        // מכשול
        b.place(6, 6, 'b', 'P');
        CHECK(bishop->isValidMove(4, 4, 7, 7, b) == false); // חסום!
    }

    SUBCASE("Queen - Both straight and diagonal") {
        b.place(4, 4, 'w', 'Q');

        // מלכה יכולה לעשות הכל
        CHECK(queen->isValidMove(4, 4, 4, 7, b) == true); // כמו צריח
        CHECK(queen->isValidMove(4, 4, 1, 1, b) == true); // כמו רץ

        // אבל גם אותה אפשר לחסום
        b.place(4, 6, 'w', 'P');
        CHECK(queen->isValidMove(4, 4, 4, 7, b) == false); // חסומה קדימה
    }
}

// ==========================================
// 5. Knight - The Jumper
// ==========================================
TEST_CASE("Knight - L-Shape and Leaping") {
    Board b(8, 8);
    const Piece* knight = PieceFactory::getPiece('N');

    b.place(4, 4, 'w', 'N');

    SUBCASE("Valid L-Shapes") {
        CHECK(knight->isValidMove(4, 4, 5, 6, b) == true);
        CHECK(knight->isValidMove(4, 4, 6, 5, b) == true);
        CHECK(knight->isValidMove(4, 4, 3, 2, b) == true);
        CHECK(knight->isValidMove(4, 4, 2, 3, b) == true);
    }

    SUBCASE("Invalid geometries") {
        CHECK(knight->isValidMove(4, 4, 4, 6, b) == false); // ישר
        CHECK(knight->isValidMove(4, 4, 6, 6, b) == false); // אלכסון נקי
    }

    SUBCASE("Knight ignores obstacles in the path") {
        // מקיפים את הסוס בכלים מכל הכיוונים
        b.place(4, 5, 'w', 'P');
        b.place(5, 4, 'b', 'P');
        b.place(5, 5, 'w', 'R');

        // הוא עדיין יכול לקפוץ מעליהם ליעד ה-L שלו!
        CHECK(knight->isValidMove(4, 4, 6, 5, b) == true);
    }
}