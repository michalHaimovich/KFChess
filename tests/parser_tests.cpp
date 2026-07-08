#include "doctest.h"
#include "parser.hpp"
#include <sstream>

// ==========================================
// 1. Happy Path
// ==========================================
TEST_CASE("Parser - Valid Full Stream") {
	std::stringstream ss(
		"Board:\n"
		"wK . bR\n"
		". bP wQ\n"
		"Commands:\n"
		"click 1 2\n"
		"jump 0 1\n"
		"wait 500\n"
		"print board\n"
	);

	ParsedInput result = Parser::parseStream(ss);

	SUBCASE("Board parsed correctly") {
		CHECK(result.board.getWidth() == 3);
		CHECK(result.board.getHeight() == 2);
		CHECK(result.board.at(0, 0).toString() == "wK");
		CHECK(result.board.at(1, 0).toString() == ".");
		CHECK(result.board.at(2, 1).toString() == "wQ");
	}

	SUBCASE("Commands parsed correctly") {
		REQUIRE(result.commands.size() == 4);

		CHECK(result.commands[0].type == CommandType::CLICK);
		CHECK(result.commands[0].arg1 == 1);
		CHECK(result.commands[0].arg2 == 2);

		CHECK(result.commands[1].type == CommandType::JUMP);
		CHECK(result.commands[1].arg1 == 0);
		CHECK(result.commands[1].arg2 == 1);

		CHECK(result.commands[2].type == CommandType::WAIT);
		CHECK(result.commands[2].arg1 == 500);

		CHECK(result.commands[3].type == CommandType::PRINT_BOARD);
	}
}

// ==========================================
// 2. Token Validation & Board Errors
// ==========================================
TEST_CASE("Parser - Invalid Board Tokens") {
	SUBCASE("Unknown piece type") {
		std::stringstream ss("Board:\nwX .\nCommands:\n");
		CHECK_THROWS_WITH_AS(Parser::parseStream(ss), "ERROR UNKNOWN_TOKEN", std::runtime_error);
	}

	SUBCASE("Invalid color") {
		std::stringstream ss("Board:\nxK .\nCommands:\n");
		CHECK_THROWS_WITH_AS(Parser::parseStream(ss), "ERROR UNKNOWN_TOKEN", std::runtime_error);
	}

	SUBCASE("Token length too long") {
		std::stringstream ss("Board:\nwKing .\nCommands:\n");
		CHECK_THROWS_WITH_AS(Parser::parseStream(ss), "ERROR UNKNOWN_TOKEN", std::runtime_error);
	}
}

TEST_CASE("Parser - Row Width Mismatch") {
	std::stringstream ss(
		"Board:\n"
		"wK .\n"
		"bP . wQ\n"
		"Commands:\n"
	);
	CHECK_THROWS_WITH_AS(Parser::parseStream(ss), "ERROR ROW_WIDTH_MISMATCH", std::runtime_error);
}

// ==========================================
// 3. Command Parsing Errors & Edge Cases
// ==========================================
TEST_CASE("Parser - Command Parsing Edge Cases") {
	SUBCASE("Missing arguments turn into UNKNOWN") {
		std::stringstream ss("Board:\n.\nCommands:\nclick 1\nwait\n");
		ParsedInput result = Parser::parseStream(ss);
		REQUIRE(result.commands.size() == 2);
		CHECK(result.commands[0].type == CommandType::UNKNOWN);
		CHECK(result.commands[1].type == CommandType::UNKNOWN);
	}

	SUBCASE("Non-numeric arguments turn into UNKNOWN") {
		std::stringstream ss("Board:\n.\nCommands:\nclick a b\nwait xyz\n");
		ParsedInput result = Parser::parseStream(ss);
		REQUIRE(result.commands.size() == 2);
		CHECK(result.commands[0].type == CommandType::UNKNOWN);
		CHECK(result.commands[1].type == CommandType::UNKNOWN);
	}

	SUBCASE("Completely unrecognized command") {
		std::stringstream ss("Board:\n.\nCommands:\nfly 1 2\n");
		ParsedInput result = Parser::parseStream(ss);
		REQUIRE(result.commands.size() == 1);
		CHECK(result.commands[0].type == CommandType::UNKNOWN);
	}
}


TEST_CASE("Parser - Structural Stream Anomalies") {
	SUBCASE("Empty stream") {
		std::stringstream ss("");
		ParsedInput result = Parser::parseStream(ss);
		CHECK(result.board.getWidth() == 0);
		CHECK(result.board.getHeight() == 0);
		CHECK(result.commands.empty() == true);
	}

	SUBCASE("Whitespace tolerance (Tabs and multiple spaces)") {
		std::stringstream ss(
			"   Board:   \n"
			"\twK    . \n"
			"  Commands: \n"
			" click   1   2  \n"
		);
		ParsedInput result = Parser::parseStream(ss);
		CHECK(result.board.getWidth() == 2);
		REQUIRE(result.commands.size() == 1);
		CHECK(result.commands[0].type == CommandType::CLICK);
	}

	SUBCASE("Case Sensitivity Traps") {
		std::stringstream ss(
			"board:\n"
			"wK .\n"
			"commands:\n"
			"click 1 2\n"
		);
		ParsedInput result = Parser::parseStream(ss);
		CHECK(result.board.getWidth() == 0);
		CHECK(result.commands.empty() == true);
	}

	SUBCASE("The std::stoi Partial Match Trap") {
		std::stringstream ss("Board:\n.\nCommands:\nclick 2abc 3xyz\n");
		ParsedInput result = Parser::parseStream(ss);
		REQUIRE(result.commands.size() == 1);
		CHECK(result.commands[0].type == CommandType::UNKNOWN);
	}

	SUBCASE("Garbage after print board") {
		std::stringstream ss("Board:\n.\nCommands:\nprint board and do stuff\n");
		ParsedInput result = Parser::parseStream(ss);
		REQUIRE(result.commands.size() == 1);
		CHECK(result.commands[0].type == CommandType::UNKNOWN);
	}
}