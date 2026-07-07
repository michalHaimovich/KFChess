# 1. הגדרת גרסת המינימום הנדרשת של CMake (גרסה 3.14 נדרשת עבור FetchContent)
cmake_minimum_required(VERSION 3.14)

project(KungFuChess CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ==========================================
# שלב א': יצירת ספריית הליבה של המשחק (Core)
# ==========================================
# אנחנו אורזים את כל הלוגיקה לספרייה נפרדת נטולת main 
add_library(kungfu_chess_core 
    src/board.cpp 
    # בהמשך נוסיף לכאן גם את src/parser.cpp
  "src/pieces.cpp")

# כל מי שישתמש בספרייה הזו יקבל גישה אוטומטית לתיקיית include
target_include_directories(kungfu_chess_core PUBLIC include)

# ==========================================
# שלב ב': קובץ ההרצה הראשי (Main App)
# ==========================================
add_executable(kungfu_chess_app src/main.cpp "src/game_engine.cpp" "tests/game_engine_tests.cpp" "src/pieces.cpp")
# מחברים את ספריית הליבה לקובץ הראשי
target_link_libraries(kungfu_chess_app PRIVATE kungfu_chess_core)

# ==========================================
# שלב ג': תשתית טסטים (Design Verification)
# ==========================================

# הבאת GTest ישירות מהמאגר הרשמי# הבאת GTest ישירות כקובץ ZIP כדי למנוע נעילות קבצים של Git ב-Windows
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
# הגדרה שמונעת מ-CMake לקמפל טסטים פנימיים של גוגל שמבזבזים זמן
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# --- הפתרון לשגיאה המפלצתית: ---
# מכריח את GTest להשתמש באותה ספריית זמן ריצה (MDd) כמו הפרויקט שלנו
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# -------------------------------

FetchContent_MakeAvailable(googletest)

# הפעלת תמיכה בטסטים בפרויקט
enable_testing()

# יצירת קובץ הרצה ייעודי רק עבור הטסטים
add_executable(kungfu_chess_tests tests/board_tests.cpp src/game_engine.cpp tests/game_engine_tests.cpp "src/pieces.cpp")

# מחברים לטסטים גם את הלוגיקה שלנו (kungfu_chess_core) וגם את GTest
target_link_libraries(kungfu_chess_tests 
    PRIVATE 
    kungfu_chess_core 
    GTest::gtest_main
)

# פקודה שמאפשרת לסביבת הפיתוח (כמו VS) לזהות את הטסטים ויזואלית
include(GoogleTest)
gtest_discover_tests(kungfu_chess_tests)