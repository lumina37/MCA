#pragma once

#include <cmath>
#include <numbers>
#include <ranges>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <tlct/config.hpp>
#include <tlct/helper/constexpr/math.hpp>

#include "helper.hpp"
#include "mca/common/defines.h"

namespace mca::_proc {

namespace rgs = std::ranges;
namespace tcfg = tlct::cfg;

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
inline void preprocess_(const TLayout& layout, const cv::Mat& src, cv::Mat& dst, const double crop_ratio)
{
    const double block_width = layout.getDiameter() * crop_ratio;
    const int block_width_i = static_cast<int>(std::ceil(block_width));

    const int canvas_width = tlct::_hp::align_up<2>(layout.getMIMaxCols() * block_width_i);
    const int canvas_height = tlct::_hp::align_up<2>(layout.getMIRows() * block_width_i);

    cv::Mat canvas(canvas_height, canvas_width, src.type());
    cv::Mat src_roi_image, dst_roi_image;

    for (const int row : rgs::views::iota(0, layout.getMIRows())) {
        for (const int col : rgs::views::iota(0, layout.getMICols(row))) {
            const cv::Point2d micenter = layout.getMICenter(row, col);
            src_roi_image = getRoiImageByCenter(src, micenter, block_width);
            dst_roi_image = getRoiImageByLeftupCorner(canvas, cv::Point(col, row) * block_width_i, block_width);
            src_roi_image.copyTo(dst_roi_image);
        }
    }

    if (layout.getRotation() != 0.0) {
        cv::transpose(canvas, dst);
    } else {
        dst = std::move(canvas);
    }
}

template MCA_API void preprocess_(const tcfg::tspc::Layout& layout, const cv::Mat& src, cv::Mat& dst,
                                  const double crop_ratio);
template MCA_API void preprocess_(const tcfg::raytrix::Layout& layout, const cv::Mat& src, cv::Mat& dst,
                                  const double crop_ratio);

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
inline cv::Mat preprocess(const TLayout& layout, const cv::Mat& src, const double crop_ratio)
{
    cv::Mat dst;
    preprocess_(layout, src, dst, crop_ratio);
    return dst;
}

template MCA_API cv::Mat preprocess(const tcfg::tspc::Layout& layout, const cv::Mat& src, const double crop_ratio);
template MCA_API cv::Mat preprocess(const tcfg::raytrix::Layout& layout, const cv::Mat& src, const double crop_ratio);

} // namespace mca::_proc

namespace mca::proc {

namespace _ = _proc;

using _::preprocess;
using _::preprocess_;

} // namespace mca::proc
