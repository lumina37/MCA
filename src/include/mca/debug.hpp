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

static void dbgDrawSoildCircle(cv::Mat& dst, cv::Point2f center, float diameter, const cv::Scalar& color) {
    cv::circle(dst, center, (int)(diameter / 2.0), color, cv::LineTypes::FILLED, cv::LineTypes::LINE_AA);
}

template <typename TArrange>
    requires tcfg::concepts::CArrange<TArrange>
void dbgDrawMicroImageEdges(const TArrange& arrange, const cv::Mat& src, cv::Mat& dst) {
    dst = src.clone();

    for (const int row : rgs::views::iota(0, arrange.getMIRows())) {
        for (const int col : rgs::views::iota(0, arrange.getMICols(row))) {
            const cv::Point2f micenter = arrange.getMICenter(row, col);
            dbgDrawCircle(dst, micenter, arrange.getDiameter());
        }
    }
}

template <typename TArrange>
    requires tcfg::concepts::CArrange<TArrange>
void dbgDrawUsedArea(const TArrange& arrange, const cv::Mat& patchsizes, int viewNum, cv::Mat& dst) {
    dst = cv::Mat::zeros(arrange.getImgSize(), CV_8UC3);

    int viewShift = viewNum / 2;

    for (const int row : rgs::views::iota(0, arrange.getMIRows())) {
        for (const int col : rgs::views::iota(0, arrange.getMICols(row))) {
            if (col >= patchsizes.cols || row >= patchsizes.rows) {
                continue;
            }

            float patchSize = patchsizes.at<uchar>(row, col);
            cv::Scalar color;
            if (patchSize + (float)viewNum > arrange.getDiameter()) {
                color = cv::Scalar(0, 0, 160);
            } else {
                color = cv::Scalar::all(255);
            }

            const cv::Point2f micenter = arrange.getMICenter(row, col);
            dbgDrawCircle(dst, micenter, arrange.getDiameter());

            for (int viewShiftX = -viewShift; viewShiftX <= viewShift; viewShiftX++) {
                for (int viewShiftY = -viewShift; viewShiftY <= viewShift; viewShiftY++) {
                    cv::Point2i shift{viewShiftX, viewShiftY};
                    dbgDrawSoildCircle(dst, micenter + (cv::Point2f)shift, patchSize, color);
                }
            }
        }
    }
}

}  // namespace mca::_dbg

namespace mca::dbg {

namespace _ = _dbg;

using _::dbgDrawCircle;
using _::dbgDrawMicroImageEdges;
using _::dbgDrawSoildCircle;
using _::dbgDrawUsedArea;

}  // namespace mca::dbg
