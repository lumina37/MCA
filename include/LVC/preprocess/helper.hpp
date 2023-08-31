#pragma once

#include <cmath>

#include <opencv2/core.hpp>

#include "LVC/common/const.h"
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
    int leftup_corner_x = static_cast<int>(center.x - width / 2.0);
    int leftup_corner_y = static_cast<int>(center.y - width / 2.0);
    int width_i = static_cast<int>(ceil(width));
    cv::Rect roi(leftup_corner_x, leftup_corner_y, width_i, width_i);
    dst = src(roi);
}

static LVC_FORCE_INLINE void getRoiImageByLeftupCorner2i(const cv::Mat& src, cv::Mat& dst, const cv::Point2i& corner,
                                                         double inscribed_square_width) noexcept
{
    int inscribed_square_width_i = static_cast<int>(ceil(inscribed_square_width));
    cv::Rect roi(corner.x, corner.y, inscribed_square_width_i, inscribed_square_width_i);
    dst = src(roi);
}

static LVC_FORCE_INLINE void edgePreMult_(cv::Mat& src, const cv::Point2d& center) noexcept
{
    double factor_up = center.y - floor(center.y);
    auto area_up = src(cv::Range(0, 1), cv::Range::all());
    area_up *= factor_up;

    double factor_down = ceil(center.y) - center.y;
    auto area_down = src(cv::Range(src.rows - 1, src.rows), cv::Range::all());
    area_down *= factor_down;

    double factor_left = center.x - floor(center.x);
    auto area_left = src(cv::Range::all(), cv::Range(0, 1));
    area_left *= factor_left;

    double factor_right = ceil(center.y) - center.y;
    auto area_right = src(cv::Range::all(), cv::Range(src.cols - 1, src.cols));
    area_right *= factor_right;
}

} // namespace lvc