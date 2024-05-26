#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "mca/config.hpp"
#include "mca/impl/debug.hpp"

using namespace std;
namespace tcfg = tlct::cfg::tspc;

int main()
{
    auto src = cv::imread("src_square.png");
    const auto calib_cfg = tcfg::CalibConfig::fromXMLPath("Boys.xml");
    const auto layout = tcfg::Layout::fromCfgAndImgsize(calib_cfg, src.size());

    cv::Mat dst;
    mca::dbg::dbgDrawMicroImageEdges(layout, src, dst);
    cv::imwrite("dbg_circles_square.png", dst);
    return 0;
}
