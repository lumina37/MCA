#pragma once

#include <cmath>

#include <opencv2/core.hpp>

#include "mca/common/const.h"
#include "mca/common/defines.h"

namespace mca {

static inline cv::Mat getRoiImageByCenter(const cv::Mat& src, const cv::Point2d& center, double width) noexcept
{
    const int startx = (int)(center.x - width / 2.0);
    const int starty = (int)(center.y - width / 2.0);
    const int width_i = std::ceil(width);
    return src({startx, starty, width_i, width_i});
}

static inline cv::Mat getRoiImageByLeftupCorner(const cv::Mat& src, const cv::Point2i& corner, double width) noexcept
{
    const auto corner_i = cv::Point2i(corner);
    const int width_i = (int)ceil(width);
    cv::Rect roi(corner_i.x, corner_i.y, width_i, width_i);
    return src(roi);
}

} // namespace mca