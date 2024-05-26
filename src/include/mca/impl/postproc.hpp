#pragma once

#include <numbers>
#include <ranges>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <tlct/config.hpp>

#include "mca/common/defines.h"
#include "mca/helper.hpp"

namespace mca {

namespace rgs = std::ranges;
namespace tcfg = tlct::cfg;

namespace _hp {

static inline void genCircleMask(cv::Mat& dst, double diameter)
{
    cv::circle(dst, {dst.cols / 2, dst.rows / 2}, static_cast<int>(diameter / 2.0), cv::Scalar::all(255.0),
               cv::LineTypes::FILLED, cv::LineTypes::LINE_AA);
}

} // namespace _hp

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
static inline void postprocess_(const TLayout& layout, const cv::Mat& src, cv::Mat& dst, const double crop_ratio)
{
    cv::Mat canvas = cv::Mat::zeros(layout.getImgSize(), src.type());

    double src_block_width = layout.getDiameter() * crop_ratio;
    int src_block_width_i = static_cast<int>(ceil(src_block_width));
    int dst_block_width_i = static_cast<int>(ceil(layout.getDiameter()));

    cv::Mat src_roi_image, dst_roi_image, src_roi_image_with_border;
    cv::Mat mask_image = cv::Mat::zeros(dst_block_width_i, dst_block_width_i, src.type());
    _hp::genCircleMask(mask_image, layout.getDiameter());

    for (const int row : rgs::views::iota(0, layout.getMIRows())) {
        for (const int col : rgs::views::iota(0, layout.getMICols(row))) {
            const cv::Point2d micenter = layout.getMICenter(row, col);

            src_roi_image =
                _hp::getRoiImageByLeftupCorner(src, cv::Point(col, row) * src_block_width_i, src_block_width);
            dst_roi_image = _hp::getRoiImageByCenter(canvas, micenter, layout.getDiameter());

            int dst_leftup_corner_x = static_cast<int>(round(micenter.x - layout.getDiameter() / 2.0));
            int dst_leftup_corner_y = static_cast<int>(round(micenter.y - layout.getDiameter() / 2.0));
            int src_leftup_corner_x = static_cast<int>(round(micenter.x - src_block_width / 2.0));
            int src_leftup_corner_y = static_cast<int>(round(micenter.y - src_block_width / 2.0));

            int left_border_width = src_leftup_corner_x - dst_leftup_corner_x;
            int top_border_width = src_leftup_corner_y - dst_leftup_corner_y;
            int right_border_width = dst_block_width_i - src_block_width_i - left_border_width;
            int bot_border_width = dst_block_width_i - src_block_width_i - top_border_width;

            cv::copyMakeBorder(src_roi_image, src_roi_image_with_border, top_border_width, bot_border_width,
                               left_border_width, right_border_width,
                               cv::BorderTypes::BORDER_REPLICATE | cv::BorderTypes::BORDER_ISOLATED);

            src_roi_image_with_border.copyTo(dst_roi_image, mask_image);
        }
    }

    if (layout.getRotation() != 0.0) {
        cv::transpose(canvas, dst);
    } else {
        dst = std::move(canvas);
    }
}

template void postprocess_(const tcfg::tspc::Layout& layout, const cv::Mat& src, cv::Mat& dst, const double crop_ratio);
template void postprocess_(const tcfg::raytrix::Layout& layout, const cv::Mat& src, cv::Mat& dst,
                           const double crop_ratio);

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
inline cv::Mat postprocess(const TLayout& layout, const cv::Mat& src, const double crop_ratio)
{
    cv::Mat dst;
    postprocess_(layout, src, dst, crop_ratio);
    return dst;
}

template cv::Mat postprocess(const tcfg::tspc::Layout& layout, const cv::Mat& src, const double crop_ratio);
template cv::Mat postprocess(const tcfg::raytrix::Layout& layout, const cv::Mat& src, const double crop_ratio);

} // namespace mca