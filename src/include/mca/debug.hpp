#pragma once

#include <ranges>

#include <opencv2/imgproc.hpp>
#include <tlct/config.hpp>

namespace mca::_dbg {

namespace rgs = std::ranges;
namespace tcfg = tlct::cfg;

static void dbgDrawCircle(cv::Mat& dst, cv::Point2f center, float diameter) {
    cv::circle(dst, center, (int)(diameter / 2.0), cv::Scalar(0, 255, 0), 1, cv::LineTypes::LINE_AA);
}

template <tcfg::concepts::CArrange TArrange>
void dbgDrawMicroImageEdges(const TArrange& arrange, const cv::Mat& src, cv::Mat& dst) {
    dst = src.clone();

    for (const int row : rgs::views::iota(0, arrange.getMIRows())) {
        for (const int col : rgs::views::iota(0, arrange.getMICols(row))) {
            const cv::Point2f micenter = arrange.getMICenter(row, col);
            dbgDrawCircle(dst, micenter, arrange.getDiameter());
        }
    }
}

}  // namespace mca::_dbg

namespace mca::dbg {

namespace _ = _dbg;

using _::dbgDrawCircle;
using _::dbgDrawMicroImageEdges;

}  // namespace mca::dbg
