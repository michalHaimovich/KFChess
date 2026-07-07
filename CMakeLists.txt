# 1. הגדרת גרסת המינימום הנדרשת של CMake (סטנדרט)
cmake_minimum_required(VERSION 3.10)

# 2. שם הפרויקט והשפה (CXX מסמל C++)
project(KungFuChess CXX)

# 3. דרישה לסטנדרט מודרני של C++ (חשוב מאוד לניהול זיכרון ותכונות חדשות)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 4. הגדרת קובץ ההרצה (Executable) שלנו והקבצים שמרכיבים אותו
add_executable(kungfu_chess_app 
    src/main.cpp 
    src/board.cpp 
    src/parser.cpp
)

# 5. הגדרת תיקיית ה-include כך שהקומפיילר ידע איפה לחפש את קבצי ה-.hpp
target_include_directories(kungfu_chess_app PRIVATE include)