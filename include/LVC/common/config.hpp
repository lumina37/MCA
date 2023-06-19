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

    static LVC_FORCE_INLINE MicroImageRanges fromConfig(const Config& cfg) noexcept;

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
    iterator(float x, float y, float interval_x, float interval_y, int index_x, int index_y, int num_x, int num_y,
             bool is_horizontal, bool is_right_shift) noexcept;

    iterator& operator++() noexcept;
    bool operator!=(const iterator& rhs) const noexcept { return index_x_ != rhs.index_x_ || index_y_ != rhs.index_y_; }
    MicroImage operator*() const noexcept { return {x_, y_, index_x_, index_y_}; }

private:
    float x_;
    float y_;
    float interval_x_;
    float interval_y_;
    int index_x_;
    int index_y_;
    int num_x_;
    int num_y_;
    bool is_horizontal_;
    bool is_right_shift_;
};

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
            num_x,           num_y,           cfg.getIsHorizontal(), cfg.getIsRightShift()};
}

LVC_FORCE_INLINE MicroImageRanges::iterator MicroImageRanges::begin() const noexcept
{
    return {start_x_, start_y_, interval_x_, interval_y_, 0, 0, num_x_, num_y_, is_horizontal_, is_right_shift_};
}

LVC_FORCE_INLINE MicroImageRanges::iterator MicroImageRanges::end() const noexcept
{
    int index_x = num_x_ * static_cast<int>(!is_horizontal_);
    int index_y = num_y_ * static_cast<int>(is_horizontal_);
    return {0.0, 0.0, interval_x_, interval_y_, index_x, index_y, num_x_, num_y_, is_horizontal_, is_right_shift_};
}

LVC_FORCE_INLINE MicroImageRanges::iterator& MicroImageRanges::iterator::operator++() noexcept
{
    if (is_horizontal_) {
        index_x_ += 1;
        x_ += interval_x_;
        if (index_x_ == num_x_) {
            index_x_ = 0;
            index_y_ += 1;
            y_ += interval_y_;
            x_ -= interval_x_ * static_cast<float>(num_x_);
            int is_odd = index_y_ & 1;
            // | is_odd | is_right_shift | right or left |
            // | ------ | -------------- | ------------- |
            // | 1      | 1              | right         |
            // | 1      | 0              | left          |
            // | 0      | 1              | left          |
            // | 0      | 0              | right         |
            x_ += static_cast<float>(((is_odd + static_cast<int>(is_right_shift_)) & 1) * (-2) + 1) *
                  (interval_x_ / 2.0f);
        }
    } else {
        // just swap x and y
        index_y_ += 1;
        y_ += interval_y_;
        if (index_y_ == num_y_) {
            index_y_ = 0;
            index_x_ += 1;
            x_ += interval_x_;
            y_ -= interval_y_ * static_cast<float>(num_y_);
            int is_odd = index_x_ & 1;
            y_ += static_cast<float>(((is_odd + static_cast<int>(is_right_shift_)) & 1) * (-2) + 1) *
                  (interval_y_ / 2.0f);
        }
    }
    return *this;
}

} // namespace lvc