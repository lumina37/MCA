#include "LVC/common/helper.hpp"

#include "LVC/common/config.hpp"

namespace lvc {

Config::Config(float diameter, int width, int height, float start_x, float start_y, bool is_right_shift,
               bool is_horizontal, float interval_x, float interval_y) noexcept
    : diameter_(diameter), width_(width), height_(height), start_x_(start_x), start_y_(start_y),
      is_horizontal_(is_horizontal), is_right_shift_(is_right_shift)
{
    if (0.0 == (interval_x + interval_y)) {
        if (is_horizontal) {
            interval_x_ = calcDefaultIntervalHorizontal(diameter);
            interval_y_ = calcDefaultIntervalVertical(diameter);
        } else {
            interval_x_ = calcDefaultIntervalVertical(diameter);
            interval_y_ = calcDefaultIntervalHorizontal(diameter);
        }
    } else {
        interval_x_ = interval_x;
        interval_y_ = interval_y;
    }
}

MicroImage::MicroImage(float x, float y, int index_x, int index_y) noexcept
    : x_(x), y_(y), index_x_(index_x), index_y_(index_y)
{}

MicroImageRanges::MicroImageRanges(float start_x, float start_y, float interval_x, float interval_y, int num_x,
                                   int num_y, bool is_horizontal = true, bool is_right_shift = true) noexcept
    : start_x_(start_x), start_y_(start_y), interval_x_(interval_x), interval_y_(interval_y), num_x_(num_x),
      num_y_(num_y), is_horizontal_(is_horizontal), is_right_shift_(is_right_shift)
{}

MicroImageRanges::iterator::iterator(float x, float y, float interval_x, float interval_y, int index_x, int index_y,
                                     int num_x, int num_y, bool is_horizontal, bool is_right_shift) noexcept
    : x_(x), y_(y), interval_x_(interval_x), interval_y_(interval_y), index_x_(index_x), index_y_(index_y),
      num_x_(num_x), num_y_(num_y), is_horizontal_(is_horizontal), is_right_shift_(is_right_shift)
{}

} // namespace lvc