﻿#pragma once

#include <opencv2/core.hpp>

#include "mca/common/defines.h"
#include "mca/config.hpp"

namespace mca {

MCA_API inline void preprocess(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    double block_width = cfg.getDiameter() * cfg.getCropRatio();
    int block_width_i = static_cast<int>(ceil(block_width));

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    auto align = [](int num) {
        constexpr int target = 2;
        int pad = (target - num % target) % target;
        return num + pad;
    };

    int req_cols = align(mis.getMaxElemNum() * block_width_i);
    int req_rows = align(mis.getMaxLineNum() * block_width_i);
    if (cfg.getIsRotated())
        std::swap(req_cols, req_rows);

    dst.create(req_rows, req_cols, src.type());
    cv::Mat src_roi_image, dst_roi_image;

    for (const auto& mi : mis) {
        getRoiImageByCenter(src, src_roi_image, mi.getCenter(), block_width);
        getRoiImageByLeftupCorner(dst, dst_roi_image, mi.getIndex() * block_width_i, block_width);
        src_roi_image.copyTo(dst_roi_image);
    }
}

} // namespace mca