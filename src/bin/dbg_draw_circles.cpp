#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <tlct.hpp>

#include "mca/impl/debug.hpp"

namespace tn = tlct::tspc;

int main(int argc, char* argv[])
{
    const auto cfg_map = tlct::ConfigMap::fromPath(argv[1]);
    const auto param_cfg = tn::ParamConfig::fromConfigMap(cfg_map);
    const auto layout = tn::Layout::fromParamConfig(param_cfg);

    auto src = cv::imread("src_square.png");

    cv::Mat dst;
    mca::dbg::dbgDrawMicroImageEdges(layout, src, dst);
    cv::imwrite("dbg_circles_square.png", dst);
    return 0;
}
