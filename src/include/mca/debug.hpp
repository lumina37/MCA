#pragma once

#include <numbers>
#include <ranges>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <tlct/config.hpp>

#include "mca/common/defines.h"

namespace mca::_dbg {

namespace rgs = std::ranges;
namespace tcfg = tlct::cfg;

static inline void dbgDrawCircle(cv::Mat& dst, cv::Point2d center, double diameter)
{
    cv::circle(dst, static_cast<cv::Point2i>(center), static_cast<int>(diameter / 2.0), cv::Scalar(0, 255, 0), 1,
               cv::LineTypes::LINE_AA);
}

static inline void dbgDrawSoildCircle(cv::Mat& dst, cv::Point2d center, double diameter, const cv::Scalar& color)
{
    cv::circle(dst, static_cast<cv::Point2i>(center), static_cast<int>(diameter / 2.0), color, cv::LineTypes::FILLED,
               cv::LineTypes::LINE_AA);
}

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
inline void dbgDrawMicroImageEdges(const TLayout& layout, const cv::Mat& src, cv::Mat& dst)
{
    dst = src.clone();

    for (const int row : rgs::views::iota(0, layout.getMIRows())) {
        for (const int col : rgs::views::iota(0, layout.getMICols(row))) {
            const cv::Point2d micenter = layout.getMICenter(row, col);
            dbgDrawCircle(dst, micenter, layout.getDiameter());
        }
    }
}

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
inline void dbgDrawUsedArea(const TLayout& layout, const cv::Mat& patchsizes, int view_num, cv::Mat& dst)
{
    dst = cv::Mat::zeros(layout.getImgSize(), CV_8UC3);

    int view_shift = view_num / 2;

    for (const int row : rgs::views::iota(0, layout.getMIRows())) {
        for (const int col : rgs::views::iota(0, layout.getMICols(row))) {
            if (col >= patchsizes.cols || row >= patchsizes.rows) {
                continue;
            }

            double patch_size = static_cast<double>(patchsizes.at<uchar>(row, col));
            cv::Scalar color;
            if (patch_size + static_cast<double>(view_num) > layout.getDiameter()) {
                color = cv::Scalar(0, 0, 160);
            } else {
                color = cv::Scalar::all(255);
            }

            const cv::Point2d micenter = layout.getMICenter(row, col);
            dbgDrawCircle(dst, micenter, layout.getDiameter());

            for (int view_shift_x = -view_shift; view_shift_x <= view_shift; view_shift_x++) {
                for (int view_shift_y = -view_shift; view_shift_y <= view_shift; view_shift_y++) {
                    cv::Point2i shift{view_shift_x, view_shift_y};
                    dbgDrawSoildCircle(dst, micenter + static_cast<cv::Point2d>(shift), patch_size, color);
                }
            }
        }
    }
}

} // namespace mca::_dbg

namespace mca::dbg {

namespace _ = _dbg;

using _::dbgDrawCircle;
using _::dbgDrawMicroImageEdges;
using _::dbgDrawSoildCircle;
using _::dbgDrawUsedArea;

} // namespace mca::dbg
