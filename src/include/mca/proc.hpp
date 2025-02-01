#pragma once

#include "mca/proc/helper.hpp"
#include "mca/proc/postproc.hpp"
#include "mca/proc/preproc.hpp"

namespace mca::proc {

namespace _ = _proc;

using _::postprocessInto;
using _::preprocessInto;
using _::preprocOutputSize;

}  // namespace mca::proc
