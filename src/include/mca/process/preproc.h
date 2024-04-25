#pragma once

#include <opencv2/core.hpp>

#include "mca/common/defines.h"
#include "mca/config/config.h"

namespace mca {

LVC_EXPORT void preprocess(const Config& cfg, const cv::Mat& src, cv::Mat& dst);

} // namespace mca