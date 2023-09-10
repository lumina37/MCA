#pragma once

#include <cmath>

#include <opencv2/core.hpp>

#include "LVC/common/const.h"
#include "LVC/common/defines.h"

namespace lvc {

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