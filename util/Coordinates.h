#pragma once

#include <vector>

namespace util {

/**
 * A Coordinate contains a row and column and is used to identify a point on a 2D surface
 */
struct Coordinate {
    using ordinate_type = std::size_t;

    ordinate_type row;
    ordinate_type column;
};

/**
 * Coordinates manages a @D surface. It provides a means of iterating through all of the
 * coordinates as well as iterating over the neighbors of a coordinate.
 */
class Coordinates {
public:
    using ordinate_type = Coordinate::ordinate_type;
    using offset_type = std::size_t;

public:
    /**
     * The Neigbors class contains all of the neighbors of a coordinate. It provides
     * a a means of iterating over those ngihbors.
     */
    class Neighbors {
    public:
        using ordinate_type = Coordinate::ordinate_type;
        using contents_type = std::vector<Coordinate>;

    public:
        Neighbors(ordinate_type row, ordinate_type col, const Coordinates& coordinates);

        auto begin() { return _neighbors.begin(); }
        auto end() { return _neighbors.end(); }

    private:
        contents_type _neighbors;
    };

public:
    /**
     * The CoodinatesIterator provides a custom iterator for iterating over all of the coordinated.
     */
    class CoordinatesIterator {
    public:
        // @todo this may not be quite right, but its close enough for what we use it for
        using self_type = CoordinatesIterator;
        using iterator_category = std::forward_iterator_tag;
        using value_type = Coordinate;
        using difference_type = Coordinate;
        using pointer = Coordinate;
        using reference = Coordinate;

    private:
        // these need to come first in order to facilitate deduced retur type deduction
        //auto toOffset(ordinate_type row, ordinate_type col) { return row * _cols + col; }
        auto fromOffset(offset_type offset) { return Coordinate { offset / _cols, offset % _cols }; }

    public:
        CoordinatesIterator(ordinate_type rows, ordinate_type cols, offset_type offset) : _rows(rows), _cols(cols), _offset(offset) {}

    public:
        auto operator++() { auto copy = *this; _offset++; return copy; }
        auto operator++([[maybe_unused]] int dummy) { _offset++; return *this; }
        reference operator*() { return fromOffset(_offset); }
        pointer operator->() { return fromOffset(_offset); }
        bool operator==(const self_type& rhs) { return _offset == rhs._offset && _rows == rhs._rows && _cols == rhs._cols; }
        bool operator!=(const self_type& rhs) { return !(*this == rhs); }

    private:
        ordinate_type _rows;
        ordinate_type _cols;
        offset_type _offset;
    };

public:
    auto rows() const { return _rows; }
    auto columns() const { return _cols; }

public:
    Coordinates(ordinate_type rows, ordinate_type cols) : _rows(rows), _cols(cols) {}

public:
    auto neighbors(ordinate_type row, ordinate_type col) const { return Neighbors(row, col, *this); }
    auto neighbors(const Coordinate& coord) const { return Neighbors(coord.row, coord.column, *this); }
    auto begin() { return CoordinatesIterator(_rows, _cols, 0); }
    auto end() { return CoordinatesIterator(_rows, _cols, _rows * _cols); }
    auto begin() const { return CoordinatesIterator(_rows, _cols, 0); }
    auto end() const { return CoordinatesIterator(_rows, _cols, _rows * _cols); }

private:
    ordinate_type _rows;
    ordinate_type _cols;
};

} // namespace util
