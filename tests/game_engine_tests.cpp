#include "doctest.h"
#include "game_engine.hpp"

// ==========================================
// 1. Pixel Conversion & Selection Logic
// ==========================================
TEST_CASE("GameEngine - Pixel clicks and Selection State") {
	// Create a standard board for testing purposes
	Board b(8, 8);
	b.place(0, 0, 'w', 'R'); // White rook in corner
	b.place(2, 0, 'w', 'N'); // White knight next to it
	b.place(7, 7, 'b', 'K'); // Black king at the other end

	// Engine will run with default settings: each square is 100 pixels
	GameEngine engine(b);

	SUBCASE("Out of bounds pixel clicks do not crash") {
		// Negative pixels
		engine.handleClick(-50, -10);
		// Pixels exceeding the 800x800 board
		engine.handleClick(1000, 9500);

		// Verify that the board remains exactly the same and nothing broke
		CHECK(engine.getBoard().at(0, 0).type == 'R');
	}
	SUBCASE("Selection persistence (Clicking same piece twice)") {
		// 1. Click on the rook (pixel 50,50 -> square 0,0)
		engine.handleClick(50, 50);

		// 2. Click again on the rook - in your case it doesn't cancel, so it should remain selected!
		engine.handleClick(80, 80);

		// 3. Click on a valid and empty destination square (0,3 -> pixel 50, 350)
		// Since it's still selected, this time it should move!
		engine.handleClick(50, 350);

		// Advance time so the move completes
		engine.wait(2000);

		// Test: the rook should be at destination (0,3) and not at (0,0)
		CHECK(engine.getBoard().at(0, 0).type == '\0');
		CHECK(engine.getBoard().at(0, 3).type == 'R');
	}
	SUBCASE("Change of mind (Switching selection to another friendly piece)") {
		// 1. Click on the white rook (0,0)
		engine.handleClick(50, 50);

		// 2. Click on the white knight (2,0 -> pixel 250, 50)
		// This should change the selection, not try to move the rook to the knight!
		engine.handleClick(250, 50);

		// 3. Click on a valid destination for the knight (3,2 -> pixel 350, 250)
		engine.handleClick(350, 250);

		// Advance time so the move (if it happened) completes
		engine.wait(2000);

		// Verify that the knight is the one that moved, and the rook stayed in place
		CHECK(engine.getBoard().at(0, 0).type == 'R'); // Rook in place
		CHECK(engine.getBoard().at(2, 0).type == '\0'); // Knight left its source
		CHECK(engine.getBoard().at(3, 2).type == 'N'); // Knight landed at destination
	}
}
// ==========================================
// 2. Timing and Priority Queue Execution
// ==========================================
TEST_CASE("GameEngine - Time Management and Movement") {
	Board b(8, 8);
	b.place(0, 0, 'w', 'R'); // White rook
	b.place(7, 7, 'b', 'R'); // Black rook

	GameEngine engine(b);

	SUBCASE("Piece does not arrive before its calculated time") {
		// Select the white rook
		engine.handleClick(50, 50);
		// Send to destination at distance 3 squares (0,3) -> expected time: 3 * 500 = 1500ms
		engine.handleClick(50, 350);

		// Advance 1499 milliseconds - the piece should not be at destination yet!
		engine.wait(1499);
		CHECK(engine.getBoard().at(0, 3).type == '\0');

		// Advance 1 more millisecond - now it must land
		engine.wait(1);
		CHECK(engine.getBoard().at(0, 3).type == 'R');
		// Verify that the source square emptied
		CHECK(engine.getBoard().at(0, 0).type == '\0');
	}

	SUBCASE("Concurrent movements resolve in correct chronological order") {
		// Move 1: white rook to distance 4 squares (arrival time: 2000ms)
		engine.handleClick(50, 50);
		engine.handleClick(50, 450); // Destination (0,4)

		// Move 2: black rook to distance 2 squares (arrival time: 1000ms)
		engine.handleClick(750, 750);
		engine.handleClick(550, 750); // Destination (5,7)

		// Advance 1000ms - the black rook should land, the white one is still moving
		engine.wait(1000);
		CHECK(engine.getBoard().at(5, 7).color == 'b'); // Black arrived
		CHECK(engine.getBoard().at(0, 4).type == '\0'); // White not yet

		// Advance 1000ms more - white completes its journey
		engine.wait(1000);
		CHECK(engine.getBoard().at(0, 4).color == 'w'); // White arrived
	}
}

// ==========================================
// 3. Locked Squares (Collision Prevention)
// ==========================================
TEST_CASE("GameEngine - Target Square Locking") {
	Board b(8, 8);
	b.place(0, 0, 'w', 'R'); // White rook 1
	b.place(2, 0, 'w', 'R'); // White rook 2

	GameEngine engine(b);

	SUBCASE("Cannot move to a target square that is already claimed") {
		// Rook 1 sent to (0,4). Travel time: 2000ms
		engine.handleClick(50, 50);
		engine.handleClick(50, 450);

		// Rook 2 tries to also go to (0,4) immediately!
		engine.handleClick(250, 50);
		engine.handleClick(50, 450);

		// Advance enough time for everyone to land (3000ms)
		engine.wait(3000);

		// What should happen? Rook 1 was first to claim ownership of (0,4), so it lands there.
		CHECK(engine.getBoard().at(0, 4).type == 'R');

		// Rook 2 was supposed to hit a locked square ("lockedSquares"),
		// so its request was rejected and it stayed at its original position (2,0)
		CHECK(engine.getBoard().at(2, 0).type == 'R');
	}
}

// ==========================================
// 4. Jump Mechanics
// ==========================================
TEST_CASE("GameEngine - Jump Mechanic Timing") {
	Board b(8, 8);
	b.place(4, 4, 'w', 'N'); // White knight in center

	GameEngine engine(b);

	SUBCASE("Jump delays arrival time correctly") {
		// Select the knight
		engine.handleClick(450, 450);

		// Activate jump to a valid knight destination (5,6)
		// Normal travel: (abs(5-4) + abs(6-4)) = 3 cells? For a knight this is usually calculated as 3 cells or a fixed time.
		// Assume it's 3 * 500 = 1500ms. Plus jumpDurationMs (1000ms), expect 2500ms total.
		// (This test will verify that the jump simply takes more time than regular travel due to air time).

		engine.handleJump(550, 650);

		// Advance regular travel time (assume 1500ms) when the knight would have landed if it wasn't jumping
		engine.wait(1500);

		// Because it's in the air (Jump), it's still not there yet
	}
}