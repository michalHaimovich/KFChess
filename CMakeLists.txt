cmake_minimum_required(VERSION 3.14)
project(KungFuChess CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ==========================================
# Phase A: Creating the game core library
# ==========================================
# שמים פה רק את קבצי הלוגיקה - בלי שום main!
add_library(kungfu_chess_core 
    src/board.cpp 
    src/pieces.cpp
    src/game_engine.cpp
    src/parser.cpp
 "tests/Pieces.cpp")

# מגדירים איפה נמצאים קבצי ה-hpp
target_include_directories(kungfu_chess_core PUBLIC include)

# ==========================================
# Phase B: Main executable (Main App)
# ==========================================
# קובץ ההרצה של המשחק. מכיל אך ורק את main.cpp
add_executable(kungfu_chess_app src/main.cpp "tests/parser_tests.cpp" "tests/Pieces.cpp")

# מחברים אליו את הלוגיקה
target_link_libraries(kungfu_chess_app PRIVATE kungfu_chess_core)

# ==========================================
# Phase C: Test infrastructure (doctest)
# ==========================================
# קובץ ההרצה של הטסטים. מכיל את ה-main של הטסטים ואת קבצי הבדיקה
add_executable(kungfu_chess_tests 
    tests/test_main.cpp 
    tests/board_tests.cpp
    tests/parser_tests.cpp
 "tests/Pieces.cpp")

# מחברים אליו את הלוגיקה
target_link_libraries(kungfu_chess_tests PRIVATE kungfu_chess_core)