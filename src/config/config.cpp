#include "LVC/preprocess/helper.hpp"

#include "LVC/config/config.h"

namespace lvc {

Config::Config(double diameter, int width, int height, const cv::Vec<cv::Point2d, 2>& start,
               double square_width_diam_ratio) noexcept
    : diameter_(diameter), width_(width), height_(height), start_(start),
      square_width_diam_ratio_(square_width_diam_ratio)
{}

MicroImage::MicroImage(double x, double y, int index_x, int index_y) noexcept
    : x_(x), y_(y), index_x_(index_x), index_y_(index_y)
{}

MicroImageRanges::MicroImageRanges(const cv::Vec<cv::Point2d, 2>& start, double interval_x, double interval_y,
                                   const cv::Vec<int, 2>& vec_num_x, int num_y) noexcept
    : start_(start), interval_x_(interval_x), interval_y_(interval_y), vec_num_x_(vec_num_x), num_y_(num_y)
{}

MicroImageRanges::iterator::iterator(const cv::Vec<cv::Point2d, 2>& start, double interval_x, double interval_y,
                                     int index_x, int index_y, const cv::Vec<int, 2>& vec_num_x, int num_y) noexcept
    : start_(start), interval_x_(interval_x), interval_y_(interval_y), index_x_(index_x), index_y_(index_y),
      vec_num_x_(vec_num_x), num_y_(num_y)
{}

MicroImageRanges MicroImageRanges::fromConfig(const Config& cfg) noexcept
{
    auto start = cfg.getStart();
    double interval_x = abs(start[1].x - start[0].x) * 2.0;
    double interval_y = start[1].y - start[0].y;

    cv::Vec<int, 2> vec_num_x;
    for (int i = 0; i < 2; i++) {
        auto start_p = start[i];
        vec_num_x[i] = static_cast<int>((cfg.getWidth() - start_p.x - cfg.getDiameter() / 2) / interval_x) + 1;
    }

    int num_y = static_cast<int>((cfg.getHeight() - start[0].y - cfg.getDiameter() / 2) / interval_y) + 1;

    return {start, interval_x, interval_y, vec_num_x, num_y};
}

MicroImageRanges::iterator MicroImageRanges::begin() const noexcept
{
    return {start_, interval_x_, interval_y_, 0, 0, vec_num_x_, num_y_};
}

MicroImageRanges::iterator MicroImageRanges::end() const noexcept
{
    return {start_, interval_x_, interval_y_, 0, num_y_, vec_num_x_, num_y_};
}

MicroImageRanges::iterator& MicroImageRanges::iterator::operator++() noexcept
{
    index_x_ += 1;
    int num_x = vec_num_x_[index_y_ % 2];
    if (index_x_ == num_x) {
        index_x_ = 0;
        index_y_ += 1;
    }
    return *this;
}

MicroImage MicroImageRanges::iterator::fromIndex(int index_x, int index_y) const noexcept
{
    int mod_idx_y = index_y % 2;
    double start_x = start_[mod_idx_y].x;
    double start_y = start_[0].x;
    double x = start_x + static_cast<double>(index_x) * interval_x_;
    double y = start_y + static_cast<double>(index_y) * interval_y_;
    return {x, y, index_x, index_y};
}

} // namespace lvc