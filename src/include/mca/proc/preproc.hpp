#pragma once

#include <cmath>
#include <numbers>
#include <ranges>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <tlct/config.hpp>
#include <tlct/helper/constexpr/math.hpp>

#include "mca/common/defines.h"
#include "mca/proc/helper.hpp"

namespace mca::_proc {

namespace rgs = std::ranges;
namespace tcfg = tlct::cfg;

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
static inline cv::Size preprocOutputSize(const TLayout& layout, const double crop_ratio) {
    const double block_width = layout.getDiameter() * crop_ratio;
    const int block_width_i = (int)std::round(block_width);

    const int canvas_width = tlct::_hp::alignUp<2>(layout.getMIMaxCols() * block_width_i);
    const int canvas_height = tlct::_hp::alignUp<2>(layout.getMIRows() * block_width_i);

    return {canvas_width, canvas_height};
}

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
static inline void preprocessInto(const TLayout& layout, const cv::Mat& src, cv::Mat& dst, const double crop_ratio) {
    const double block_width = layout.getDiameter() * crop_ratio;
    const int block_width_i = (int)std::round(block_width);
    const auto& canvas_size = preprocOutputSize(layout, crop_ratio);

    dst.create(canvas_size, src.type());
    cv::Mat src_roi_image, dst_roi_image;

    for (const int row : rgs::views::iota(0, layout.getMIRows())) {
        for (const int col : rgs::views::iota(0, layout.getMICols(row))) {
            const cv::Point2d micenter = layout.getMICenter(row, col);
            src_roi_image = getRoiImageByCenter(src, micenter, block_width);
            dst_roi_image = getRoiImageByLeftupCorner(dst, cv::Point(col, row) * block_width_i, block_width);
            src_roi_image.copyTo(dst_roi_image);
        }
    }
}

}  // namespace mca::_proc
