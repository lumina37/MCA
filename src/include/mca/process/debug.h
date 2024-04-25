#pragma once

#include <opencv2/core.hpp>

#include "mca/common/defines.h"
#include "mca/config.h"

namespace mca {

LVC_EXPORT void dbgDrawMicroImageEdges(const Config& cfg, const cv::Mat& src, cv::Mat& dst);

LVC_EXPORT void dbgDrawUsedArea(const Config& cfg, const cv::Mat& patch_size_map, int view_num, cv::Mat& dst);

} // namespace mca