#include "board.hpp"

// Initializer List: initializes the vector in advance to full size
// with empty squares to prevent unnecessary memory allocations during runtime
Board::Board(size_t width, size_t height)
    : width_(width), height_(height), grid_(width* height, { '.', '\0' }) {
}

size_t Board::getWidth() const {
    return width_;
}

size_t Board::getHeight() const {
    return height_;
}

// Calculates the 1D index from 2D coordinates
size_t Board::getIndex(size_t x, size_t y) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Coordinates out of board bounds");
    }
    return y * width_ + x;
}

Board::Square Board::at(size_t x, size_t y) const {
    return grid_[getIndex(x, y)];
}

void Board::place(size_t x, size_t y, char color, char type) {
    grid_[getIndex(x, y)] = { color, type };
}

void Board::place(size_t x, size_t y, const Square& square) {
    grid_[getIndex(x, y)] = square;
}

std::string Board::toCanonicalString() const {
    std::string result;

    for (size_t y = 0; y < height_; ++y) {
        for (size_t x = 0; x < width_; ++x) {
            result += at(x, y).toString();

            // Add space between pieces, except at end of row
            if (x < width_ - 1) {
                result += " ";
            }
        }
        // Newline at end of each board row
        result += "\n";
    }

    return result;
}