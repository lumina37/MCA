#pragma once

#include <cmath>

#include <opencv2/core.hpp>

#include "LVC/common/const.h"
#include "LVC/common/defines.h"

namespace lvc {

static inline LVC_CONSTFUNC double calcDefaultIntervalHorizontal(double diameter) noexcept { return diameter; }

static inline LVC_CONSTFUNC double calcDefaultIntervalVertical(double diameter) noexcept { return dSQRT3_2 * diameter; }

static inline LVC_CONSTFUNC int calcNumHorizontal(int width, double diameter) noexcept
{
    double num_x = static_cast<double>(width) / diameter;
    return static_cast<int>(std::round(num_x)) - 1;
}

static inline LVC_CONSTFUNC int calcNumVertical(int height, double diameter) noexcept
{
    double num_y = static_cast<double>(height) / (diameter * dSQRT3_2);
    return static_cast<int>(std::round(num_y)) - 1;
}

static inline void getRoiImageByCenter(const cv::Mat& src, cv::Mat& dst, const cv::Point2d& center,
                                       double width) noexcept
{
    int leftup_corner_x = static_cast<int>(round(center.x - width / 2.0));
    int leftup_corner_y = static_cast<int>(round(center.y - width / 2.0));
    int width_i = static_cast<int>(round(width));
    cv::Rect roi(leftup_corner_x, leftup_corner_y, width_i, width_i);
    dst = src(roi);
}

static inline void getRoiImageByLeftupCorner(const cv::Mat& src, cv::Mat& dst, const cv::Point2i& corner,
                                             double width) noexcept
{
    cv::Point2i corner_i = static_cast<cv::Point2i>(corner);
    int width_i = static_cast<int>(round(width));
    cv::Rect roi(corner_i.x, corner_i.y, width_i, width_i);
    dst = src(roi);
}

} // namespace lvc