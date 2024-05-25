#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <tlct/config.hpp>

#include "mca/impl/preproc.hpp"

namespace tcfg = tlct::cfg::raytrix;

int main()
{
    const cv::Mat src = cv::imread("NagoyaFujita.png");
    const auto config = tcfg::CalibConfig::fromXMLPath("NagoyaFujita.xml");
    const auto layout = tcfg::Layout::fromCfgAndImgsize(config, src.size());
    const cv::Mat resized_img = procImg(layout, src);

    const cv::Mat dst = mca::preprocess(layout, resized_img);
    cv::imwrite("forward.png", dst);
}
