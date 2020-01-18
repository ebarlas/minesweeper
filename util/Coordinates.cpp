#include "Coordinates.h"

namespace util {
Coordinates::Neighbors::Neighbors(ordinate_type row, ordinate_type col, const Coordinates& coords) {
    auto beginRow = row == 0 ? row : row-1;
    auto endRow = row == coords.rows()-1 ? row : row+1;
    auto beginCol = col == 0 ? col : col-1;
    auto endCol = col == coords.columns()-1 ? col : col+1;
    for (auto r = beginRow; r <= endRow; ++r) {
        for (auto c = beginCol; c <= endCol; ++c) {
            if (!(row == r && col == c))
                _neighbors.emplace_back(Coordinate{r,c});
        }
    }
}

} // namespace util
