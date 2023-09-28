#include "LVC/preprocess/helper.hpp"

#include "LVC/config/config.h"

namespace lvc {

Config::Config(double diameter, int width, int height, cv::Point2d point, double square_width_diam_ratio,
               bool is_rotated) noexcept
    : diameter_(diameter), width_(width), height_(height), square_width_diam_ratio_(square_width_diam_ratio),
      is_rotated_(is_rotated)
{
    double radius = diameter / 2.0;
    // `line` is ALWAYS contiguous.
    // If `is_rotated==false`, just like *chess*, then `line` is horizontal.
    // Otherwise, aiming to keep `line` contiguous, it is vertical.
    double elem_interval = diameter;
    double line_interval = diameter * dSQRT3_2;

    if (!is_rotated) {
        int line_idx = static_cast<int>((point.y - radius) / line_interval);
        line_starts_[line_idx % 2].x = point.x - floor((point.x - radius) / elem_interval) * elem_interval;
        line_starts_[abs((line_idx % 2) - 1)].x = point.x - floor(point.x / elem_interval) * elem_interval + radius;
        line_starts_[0].y = point.y - line_idx * line_interval;
        line_starts_[1].y = line_starts_[0].y + line_interval;
    } else {
        int line_idx = static_cast<int>((point.x - radius) / line_interval);
        line_starts_[line_idx % 2].x = point.y - floor((point.y - radius) / elem_interval) * elem_interval;
        line_starts_[abs((line_idx % 2) - 1)].x = point.y - floor(point.y / elem_interval) * elem_interval + radius;
        line_starts_[0].y = point.x - line_idx * line_interval;
        line_starts_[1].y = line_starts_[0].y + line_interval;
    }
}

MicroImage::MicroImage(double x, double y, int xidx, int yidx) noexcept : x_(x), y_(y), index_x_(xidx), index_y_(yidx)
{}

MicroImageRanges::MicroImageRanges(cv::Vec<cv::Point2d, 2> line_starts, cv::Vec2i elem_nums, int line_num,
                                   bool is_rotated) noexcept
    : line_starts_(line_starts), elem_nums_(elem_nums), line_num_(line_num), is_rotated_(is_rotated)
{}

MicroImageRanges::iterator::iterator(cv::Vec<cv::Point2d, 2> line_starts, double elem_interval, double line_interval,
                                     int elem_idx, int line_idx, cv::Vec2i elem_nums, int line_num,
                                     bool is_rotated) noexcept
    : line_starts_(line_starts), elem_interval_(elem_interval), line_interval_(line_interval), elem_idx_(elem_idx),
      line_idx_(line_idx), elem_nums_(elem_nums), line_num_(line_num), is_rotated_(is_rotated)
{}

MicroImageRanges MicroImageRanges::fromConfig(const Config& cfg) noexcept
{
    auto line_starts = cfg.getLineStarts();
    double elem_interval = computeElemInterval(line_starts);
    double line_interval = computeLineInterval(line_starts);

    cv::Vec2i elem_nums;
    double elem_size_limit = cfg.getIsRotated() ? cfg.getHeight() : cfg.getWidth();
    double line_size_limit = cfg.getIsRotated() ? cfg.getWidth() : cfg.getHeight();

    for (int i = 0; i < 2; i++) {
        elem_nums[i] = static_cast<int>((elem_size_limit - line_starts[i].x - cfg.getDiameter() / 2) / elem_interval) + 1;
    }
    int line_num = static_cast<int>((line_size_limit - line_starts[0].y - cfg.getDiameter() / 2) / line_interval) + 1;

    return {line_starts, elem_nums, line_num, cfg.getIsRotated()};
}

MicroImageRanges::iterator MicroImageRanges::begin() const noexcept
{
    return {
        line_starts_, computeElemInterval(line_starts_), computeLineInterval(line_starts_), 0, 0, elem_nums_, line_num_,
        is_rotated_};
}

MicroImageRanges::iterator MicroImageRanges::end() const noexcept
{
    return {line_starts_, 0.0, 0.0, 0, line_num_, elem_nums_, line_num_, is_rotated_};
}

MicroImageRanges::iterator& MicroImageRanges::iterator::operator++() noexcept
{
    elem_idx_ += 1;
    int elem_num = elem_nums_[line_idx_ % 2];
    if (elem_idx_ == elem_num) {
        elem_idx_ = 0;
        line_idx_ += 1;
    }
    return *this;
}

MicroImage MicroImageRanges::iterator::fromIndex(int elem_idx, int line_idx) const noexcept
{
    int line_idx_mod2 = line_idx % 2;
    double start_x = line_starts_[line_idx_mod2].x;
    double start_y = line_starts_[0].y;
    double x = start_x + static_cast<double>(elem_idx) * elem_interval_;
    double y = start_y + static_cast<double>(line_idx) * line_interval_;

    if (is_rotated_) {
        std::swap(x, y);
        std::swap(elem_idx, line_idx);
    }

    return {x, y, elem_idx, line_idx};
}

} // namespace lvc