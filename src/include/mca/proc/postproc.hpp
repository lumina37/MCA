#pragma once

#include <cmath>
#include <ranges>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <tlct/config.hpp>

#include "mca/proc/helper.hpp"

namespace mca::_proc {

namespace rgs = std::ranges;
namespace tcfg = tlct::cfg;

static inline void genCircleMask(cv::Mat& dst, float diameter) {
    cv::circle(dst, {dst.cols / 2, dst.rows / 2}, (int)(diameter / 2.0), cv::Scalar::all(255.0), cv::LineTypes::FILLED,
               cv::LineTypes::LINE_AA);
}

template <typename TArrange>
    requires tcfg::concepts::CArrange<TArrange>
static inline void postprocessInto(const TArrange& arrange, const cv::Mat& src, cv::Mat& dst, const float cropRatio) {
    const int border = (int)(arrange.getDiameter() / 2.0);
    const cv::Point2f posShift{(float)border, (float)border};
    cv::Mat canvas = cv::Mat::zeros(arrange.getImgSize() + cv::Size(border, border) * 2, src.type());

    const float srcBlockWidth = arrange.getDiameter() * cropRatio;
    const int iSrcBlockWidth = (int)std::round(srcBlockWidth);
    const int iDstBlockWidth = (int)std::round(arrange.getDiameter());

    cv::Mat srcRoiImage, dstRoiImage, srcRoiImageWithBorder;
    cv::Mat maskImage = cv::Mat::zeros(iDstBlockWidth, iDstBlockWidth, src.type());
    genCircleMask(maskImage, arrange.getDiameter());

    for (const int row : rgs::views::iota(0, arrange.getMIRows())) {
        for (const int col : rgs::views::iota(0, arrange.getMICols(row))) {
            const cv::Point2f micenter = arrange.getMICenter(row, col) + posShift;

            srcRoiImage = getRoiImageByLeftupCorner(src, cv::Point(col, row) * iSrcBlockWidth, srcBlockWidth);
            dstRoiImage = getRoiImageByCenter(canvas, micenter, arrange.getDiameter());

            const int dstLTopX = (int)std::round(micenter.x - arrange.getDiameter() / 2.0);
            const int dstLTopY = (int)std::round(micenter.y - arrange.getDiameter() / 2.0);
            const int srcLTopX = (int)std::round(micenter.x - srcBlockWidth / 2.0);
            const int srcLTopY = (int)std::round(micenter.y - srcBlockWidth / 2.0);

            const int leftBorderWidth = srcLTopX - dstLTopX;
            const int topBorderY = srcLTopY - dstLTopY;
            const int rightBorderWidth = iDstBlockWidth - iSrcBlockWidth - leftBorderWidth;
            const int botBorderWidth = iDstBlockWidth - iSrcBlockWidth - topBorderY;

            cv::copyMakeBorder(srcRoiImage, srcRoiImageWithBorder, topBorderY, botBorderWidth, leftBorderWidth,
                               rightBorderWidth, cv::BorderTypes::BORDER_REPLICATE | cv::BorderTypes::BORDER_ISOLATED);

            srcRoiImageWithBorder.copyTo(dstRoiImage, maskImage);
        }
    }

    const cv::Mat canvasWithoutBorder = canvas({border, canvas.rows - border}, {border, canvas.cols - border});
    canvasWithoutBorder.copyTo(dst);
}

}  // namespace mca::_proc
