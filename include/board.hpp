#pragma once 

#include <vector>
#include <string>
#include <stdexcept> // For exceptions like std::out_of_range

class Board {
public:
    // Definition of the internal structure of a square
    struct Square {
        char color; // 'w', 'b', or '.'
        char type;  // 'K', 'Q', 'R', etc.

        // Helper function to convert square to string (according to VPL requirements)
        std::string toString() const {
            if (color == '.') return ".";
            return std::string(1, color) + std::string(1, type);
        }
    };

    // Constructor: receives dimensions and initializes empty board
    Board(size_t width, size_t height);

    // Accessor functions (Getters)
    size_t getWidth() const;
    size_t getHeight() const;

    // Read a square from specific position (safe from boundary violations)
    Square at(size_t x, size_t y) const;

    // Place a square/piece at specific position
    void place(size_t x, size_t y, char color, char type);

    // This function receives a ready square instead of separate parameters
    void place(size_t x, size_t y, const Square& square);

    // Returns the entire board as a single string in exactly the required format
    std::string toCanonicalString() const;

private:
    size_t width_;
    size_t height_;

    // The actual container: a contiguous 1D array for maximum cache efficiency
    std::vector<Square> grid_;

    // Internal helper function to calculate index from 2D coordinates
    size_t getIndex(size_t x, size_t y) const;
};