#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <tlct/config.hpp>

#include "mca/impl/postproc.hpp"

namespace tcfg = tlct::cfg::raytrix;

int main()
{
    const cv::Mat src = cv::imread("forward.png");
    const auto config = tcfg::CalibConfig::fromXMLPath("NagoyaFujita.xml");
    const auto layout = tcfg::Layout::fromCfgAndImgsize(config, src.size());

    const cv::Mat dst = mca::postprocess(layout, src);
    cv::imwrite("forward_backward.png", dst);
}
