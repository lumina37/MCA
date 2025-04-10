#pragma once

#include <string>

#include "mca/common/config.h"

namespace mca {

constexpr std::string_view compileInfo{MCA_COMPILE_INFO};
constexpr std::string_view version{MCA_VERSION};

}  // namespace mca
