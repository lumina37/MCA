#pragma once

#include <cmath>

#include <opencv2/core.hpp>

#include "LVC/common/const.hpp"
#include "LVC/common/defines.h"

namespace lvc {

static LVC_CONSTFUNC LVC_FORCE_INLINE double calcDefaultIntervalHorizontal(double diameter) noexcept
{
    return diameter;
}

static LVC_CONSTFUNC LVC_FORCE_INLINE double calcDefaultIntervalVertical(double diameter) noexcept
{
    return dSQRT3_2 * diameter;
}

static LVC_CONSTFUNC LVC_FORCE_INLINE int calcNumHorizontal(int width, double diameter) noexcept
{
    double num_x = static_cast<double>(width) / diameter;
    return static_cast<int>(std::round(num_x)) - 1;
}

static LVC_CONSTFUNC LVC_FORCE_INLINE int calcNumVertical(int height, double diameter) noexcept
{
    double num_y = static_cast<double>(height) / (diameter * dSQRT3_2);
    return static_cast<int>(std::round(num_y)) - 1;
}

static LVC_FORCE_INLINE void getRoiImageByCenter(const cv::Mat& src, cv::Mat& dst, const cv::Point2d& center,
                                                 double width) noexcept
{
    int leftup_corner_x = static_cast<int>(round(center.x - width / 2.0f));
    int leftup_corner_y = static_cast<int>(round(center.y - width / 2.0f));
    int width_i = static_cast<int>(round(width));
    cv::Rect roi(leftup_corner_x, leftup_corner_y, width_i, width_i);
    dst = src(roi);
}

static LVC_FORCE_INLINE void getRoiImageByLeftupCorner2i(const cv::Mat& src, cv::Mat& dst, const cv::Point2i& corner,
                                                         double inscribed_square_width) noexcept
{
    cv::Point2i corner_i = static_cast<cv::Point2i>(corner);
    int inscribed_square_width_i = static_cast<int>(round(inscribed_square_width));
    cv::Rect roi(corner_i.x, corner_i.y, inscribed_square_width_i, inscribed_square_width_i);
    dst = src(roi);
}

} // namespace lvc