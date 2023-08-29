﻿#pragma once

#include <opencv2/core.hpp>

#include "LVC/common/defines.h"
#include "LVC/config/config.h"

namespace lvc {

LVC_EXPORT void preprocessForward(const Config& cfg, const cv::Mat& src, cv::Mat& dst);

} // namespace lvc