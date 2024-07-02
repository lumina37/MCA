#pragma once

#include <cmath>
#include <numbers>
#include <ranges>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <tlct/config.hpp>

#include "helper.hpp"
#include "mca/common/defines.h"

namespace mca::_proc {

namespace rgs = std::ranges;
namespace tcfg = tlct::cfg;

static inline void genCircleMask(cv::Mat& dst, double diameter)
{
    cv::circle(dst, {dst.cols / 2, dst.rows / 2}, static_cast<int>(diameter / 2.0), cv::Scalar::all(255.0),
               cv::LineTypes::FILLED, cv::LineTypes::LINE_AA);
}

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
MCA_API inline void postprocess_(const TLayout& layout, const cv::Mat& src, cv::Mat& dst, const double crop_ratio)
{
    const int border = (int)(layout.getDiameter() / 2.0);
    const cv::Point2d pos_shift{(double)border, (double)border};
    cv::Mat canvas = cv::Mat::zeros(layout.getImgSize() + cv::Size(border, border) * 2, src.type());

    const double src_block_width = layout.getDiameter() * crop_ratio;
    const int src_block_width_i = (int)std::ceil(src_block_width);
    const int dst_block_width_i = (int)std::ceil(layout.getDiameter());

    cv::Mat src_roi_image, dst_roi_image, src_roi_image_with_border;
    cv::Mat mask_image = cv::Mat::zeros(dst_block_width_i, dst_block_width_i, src.type());
    genCircleMask(mask_image, layout.getDiameter());

    for (const int row : rgs::views::iota(0, layout.getMIRows())) {
        for (const int col : rgs::views::iota(0, layout.getMICols(row))) {
            const cv::Point2d micenter = layout.getMICenter(row, col) + pos_shift;

            src_roi_image = getRoiImageByLeftupCorner(src, cv::Point(col, row) * src_block_width_i, src_block_width);
            dst_roi_image = getRoiImageByCenter(canvas, micenter, layout.getDiameter());

            const int dst_ltop_x = (int)(micenter.x - layout.getDiameter() / 2.0);
            const int dst_ltop_y = (int)(micenter.y - layout.getDiameter() / 2.0);
            const int src_ltop_x = (int)(micenter.x - src_block_width / 2.0);
            const int src_ltop_y = (int)(micenter.y - src_block_width / 2.0);

            const int left_border_width = src_ltop_x - dst_ltop_x;
            const int top_border_width = src_ltop_y - dst_ltop_y;
            const int right_border_width = dst_block_width_i - src_block_width_i - left_border_width;
            const int bot_border_width = dst_block_width_i - src_block_width_i - top_border_width;

            cv::copyMakeBorder(src_roi_image, src_roi_image_with_border, top_border_width, bot_border_width,
                               left_border_width, right_border_width,
                               cv::BorderTypes::BORDER_REPLICATE | cv::BorderTypes::BORDER_ISOLATED);

            src_roi_image_with_border.copyTo(dst_roi_image, mask_image);
        }
    }

    const cv::Mat canvas_withoutborder = canvas({border, canvas.rows - border}, {border, canvas.cols - border});

    if (layout.getRotation() != 0.0) {
        cv::transpose(canvas_withoutborder, dst);
    } else {
        dst = std::move(canvas_withoutborder);
    }
}

template MCA_API void postprocess_(const tcfg::tspc::Layout& layout, const cv::Mat& src, cv::Mat& dst,
                                   const double crop_ratio);
template MCA_API void postprocess_(const tcfg::raytrix::Layout& layout, const cv::Mat& src, cv::Mat& dst,
                                   const double crop_ratio);

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
MCA_API inline cv::Mat postprocess(const TLayout& layout, const cv::Mat& src, const double crop_ratio)
{
    cv::Mat dst;
    postprocess_(layout, src, dst, crop_ratio);
    return dst;
}

template MCA_API cv::Mat postprocess(const tcfg::tspc::Layout& layout, const cv::Mat& src, const double crop_ratio);
template MCA_API cv::Mat postprocess(const tcfg::raytrix::Layout& layout, const cv::Mat& src, const double crop_ratio);

} // namespace mca::_proc

namespace mca::proc {

namespace _priv = mca::_proc;

using _priv::postprocess;
using _priv::postprocess_;

} // namespace mca::proc
