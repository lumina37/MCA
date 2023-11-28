﻿#pragma once

#include <opencv2/core.hpp>

#include "MCA/common/defines.h"
#include "MCA/config/config.h"

namespace mca {

LVC_EXPORT void preprocess(const Config& cfg, const cv::Mat& src, cv::Mat& dst);

} // namespace mca