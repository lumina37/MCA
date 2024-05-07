#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "mca/config.hpp"
#include "mca/impl/postproc.hpp"

using namespace std;

int main()
{
    auto cfg = mca::Config::fromRaytrixCfgFilePath("param.cfg");
    auto src = cv::imread("forward.png");
    cv::Mat dst;
    mca::postprocess(cfg, src, dst);
    cv::imwrite("forward_backward.png", dst);
    return 0;
}
