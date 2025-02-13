#pragma once

#include <cmath>
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

template <typename TArrange>
    requires tcfg::concepts::CArrange<TArrange>
static inline cv::Size preprocOutputSize(const TArrange& arrange, const float cropRatio) {
    const float blockWidth = arrange.getDiameter() * cropRatio;
    const int iBlockWidth = (int)std::round(blockWidth);

    const int canvasWidth = tlct::_hp::alignUp<2>(arrange.getMIMaxCols() * iBlockWidth);
    const int canvasHeight = tlct::_hp::alignUp<2>(arrange.getMIRows() * iBlockWidth);

    return {canvasWidth, canvasHeight};
}

template <typename TArrange>
    requires tcfg::concepts::CArrange<TArrange>
static inline void preprocessInto(const TArrange& arrange, const cv::Mat& src, cv::Mat& dst, const float cropRatio) {
    const float blockWidth = arrange.getDiameter() * cropRatio;
    const int iBlockWidth = (int)std::round(blockWidth);
    const auto& canvasSize = preprocOutputSize(arrange, cropRatio);

    dst.create(canvasSize, src.type());
    cv::Mat srcRoiImage, dstRoiImage;

    for (const int row : rgs::views::iota(0, arrange.getMIRows())) {
        for (const int col : rgs::views::iota(0, arrange.getMICols(row))) {
            const cv::Point2f micenter = arrange.getMICenter(row, col);
            srcRoiImage = getRoiImageByCenter(src, micenter, blockWidth);
            dstRoiImage = getRoiImageByLeftupCorner(dst, cv::Point(col, row) * iBlockWidth, blockWidth);
            srcRoiImage.copyTo(dstRoiImage);
        }
    }
}

}  // namespace mca::_proc
