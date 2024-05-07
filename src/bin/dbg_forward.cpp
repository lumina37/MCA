#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "mca/config.hpp"
#include "mca/impl/preproc.hpp"

int main()
{
    auto cfg = mca::Config::fromRaytrixCfgFilePath("param.cfg");
    auto src = cv::imread("src_chess.png");
    cv::Mat dst;
    mca::preprocess(cfg, src, dst);
    cv::imwrite("forward.png", dst);
    return 0;
}
