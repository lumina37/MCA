#include "LVC/common/helper.hpp"

#include "LVC/common/config.hpp"

namespace lvc {

Config::Config(float diameter, int width, int height, float start_x, float start_y, bool is_right_shift,
               bool is_horizontal, float interval_x, float interval_y, float square_width_diam_ratio) noexcept
    : diameter_(diameter), width_(width), height_(height), start_x_(start_x), start_y_(start_y),
      is_horizontal_(is_horizontal), is_right_shift_(is_right_shift), square_width_diam_ratio_(square_width_diam_ratio)
{
    if (!(interval_x > 0.0 && interval_y > 0.0)) {
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
                                   int num_y, bool is_right_shift = true, bool is_horizontal = true) noexcept
    : start_x_(start_x), start_y_(start_y), interval_x_(interval_x), interval_y_(interval_y), num_x_(num_x),
      num_y_(num_y), is_horizontal_(is_horizontal), is_right_shift_(is_right_shift)
{}

MicroImageRanges::iterator::iterator(float start_x, float start_y, float interval_x, float interval_y, int index_x,
                                     int index_y, int num_x, int num_y, bool is_horizontal,
                                     bool is_right_shift) noexcept
    : start_x_(start_x), start_y_(start_y), interval_x_(interval_x), interval_y_(interval_y), index_x_(index_x),
      index_y_(index_y), num_x_(num_x), num_y_(num_y), is_horizontal_(is_horizontal), is_right_shift_(is_right_shift)
{}

MicroImageRanges MicroImageRanges::fromConfig(const Config& cfg) noexcept
{
    int num_x, num_y;
    if (cfg.getIsHorizontal()) {
        num_x = calcNumHorizontal(cfg.getWidth(), cfg.getDiameter());
        num_y = calcNumVertical(cfg.getHeight(), cfg.getDiameter());
    } else {
        num_x = calcNumVertical(cfg.getWidth(), cfg.getDiameter());
        num_y = calcNumHorizontal(cfg.getHeight(), cfg.getDiameter());
    }

    return {cfg.getStartX(), cfg.getStartY(), cfg.getIntervalX(),    cfg.getIntervalY(),
            num_x,           num_y,           cfg.getIsRightShift(), cfg.getIsHorizontal()};
}

MicroImageRanges::iterator MicroImageRanges::begin() const noexcept
{
    return {start_x_, start_y_, interval_x_, interval_y_, 0, 0, num_x_, num_y_, is_horizontal_, is_right_shift_};
}

MicroImageRanges::iterator MicroImageRanges::end() const noexcept
{
    return {start_x_, start_y_, interval_x_, interval_y_, 0, num_y_, num_x_, num_y_, is_horizontal_, is_right_shift_};
}

MicroImageRanges::iterator& MicroImageRanges::iterator::operator++() noexcept
{
    index_x_ += 1;
    if (index_x_ == num_x_) {
        index_x_ = 0;
        index_y_ += 1;
    }
    return *this;
}

MicroImage MicroImageRanges::iterator::fromIndex(int index_x, int index_y) const noexcept
{
    if (is_horizontal_) {
        int is_odd = index_y_ & 1;
        // | is_odd | is_right_shift | shift            |
        // | ------ | -------------- | ---------------- |
        // | 1      | 1              | shift right (+)  |
        // | 1      | 0              | shift left (-)   |
        // | 0      | 1              | no shift         |
        // | 0      | 0              | no shift         |
        int shift_flag = static_cast<int>(is_right_shift_) * 2 - 1;
        float x_shift = static_cast<float>(is_odd * shift_flag) * interval_x_ / 2.0f;
        float x = start_x_ + static_cast<float>(index_x) * interval_x_ + x_shift;
        float y = start_y_ + static_cast<float>(index_y) * interval_y_;
        return {x, y, index_x, index_y};
    } else {
        // just swap x and y
        int is_odd = index_x_ & 1;
        // | is_odd | is_right_shift | shift          |
        // | ------ | -------------- | -------------- |
        // | 1      | 1              | shift down (+) |
        // | 1      | 0              | shift up (-)   |
        // | 0      | 1              | no shift       |
        // | 0      | 0              | no shift       |
        int shift_flag = static_cast<int>(is_right_shift_) * 2 - 1;
        float y_shift = static_cast<float>(is_odd * shift_flag) * interval_y_ / 2.0f;
        float y = start_y_ + static_cast<float>(index_y) * interval_y_ + y_shift;
        float x = start_x_ + static_cast<float>(index_x) * interval_x_;
        return {x, y, index_x, index_y};
    }
}

} // namespace lvc