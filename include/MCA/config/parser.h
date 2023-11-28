#pragma once

#include <string>

#include "config.h"

namespace mca {

LVC_EXPORT Config fromRaytrixCfgFilePath(const std::string& cfg_file_path);

}