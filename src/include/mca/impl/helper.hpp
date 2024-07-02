#pragma once

#include <tlct/convert/helper/roi.hpp>

namespace mca::_proc {

namespace _priv = tlct::_cvt;

using _priv::getRoiImageByCenter;
using _priv::getRoiImageByLeftupCorner;

} // namespace mca::_proc
