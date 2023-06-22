#pragma once

#include <opencv2/core.hpp>

#include "LVC/common/defines.h"
#include "LVC/common/helper.hpp"

namespace lvc {

class LVC_EXPORT Config
{
public:
    Config(float diameter, int width, int height, float start_x, float start_y, bool is_right_shift = true,
           bool is_horizontal = true, float interval_x = 0.0, float interval_y = 0.0,
           float square_width_diam_ratio = f1_SQRT2) noexcept;

    float getDiameter() const noexcept { return diameter_; };
    int getWidth() const noexcept { return width_; };
    int getHeight() const noexcept { return height_; };
    float getStartX() const noexcept { return start_x_; };
    float getStartY() const noexcept { return start_y_; };
    float getIntervalX() const noexcept { return interval_x_; };
    float getIntervalY() const noexcept { return interval_y_; };
    float getSquareWidthRatio() const noexcept { return square_width_diam_ratio_; };
    bool getIsRightShift() const noexcept { return is_right_shift_; };
    bool getIsHorizontal() const noexcept { return is_horizontal_; };

private:
    float diameter_;
    int width_;
    int height_;
    float start_x_;
    float start_y_;
    float interval_x_;
    float interval_y_;
    float square_width_diam_ratio_;
    bool is_horizontal_;
    bool is_right_shift_;
};

class LVC_EXPORT MicroImage
{
public:
    MicroImage(float x, float y, int index_x, int index_y) noexcept;

    cv::Point2f getCenter() const noexcept { return {x_, y_}; };
    cv::Point2i getIndex() const noexcept { return {index_x_, index_y_}; };

    const float x_;
    const float y_;
    const int index_x_;
    const int index_y_;
};

class LVC_EXPORT MicroImageRanges
{
public:
    MicroImageRanges(float start_x, float start_y, float interval_x, float interval_y, int num_x, int num_y,
                     bool is_horizontal, bool is_right_shift) noexcept;

    class iterator;

    static MicroImageRanges fromConfig(const Config& cfg) noexcept;

    iterator begin() const noexcept;
    iterator end() const noexcept;

    int getNumX() const noexcept { return num_x_; }
    int getNumY() const noexcept { return num_y_; }

private:
    float start_x_;
    float start_y_;
    float interval_x_;
    float interval_y_;
    int num_x_;
    int num_y_;
    bool is_horizontal_;
    bool is_right_shift_;
};

class MicroImageRanges::iterator
{
public:
    iterator(float start_x, float start_y, float interval_x, float interval_y, int index_x, int index_y, int num_x,
             int num_y, bool is_horizontal, bool is_right_shift) noexcept;

    MicroImage fromIndex(int index_x, int index_y) const noexcept;

    iterator& operator++() noexcept;
    bool operator!=(const iterator& rhs) const noexcept { return index_x_ != rhs.index_x_ || index_y_ != rhs.index_y_; }
    MicroImage operator*() const noexcept { return fromIndex(index_x_, index_y_); }

private:
    float start_x_;
    float start_y_;
    float interval_x_;
    float interval_y_;
    int index_x_;
    int index_y_;
    int num_x_;
    int num_y_;
    bool is_horizontal_;
    bool is_right_shift_;
};

} // namespace lvc