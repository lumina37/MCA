#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "mca/config.hpp"
#include "mca/impl/debug.hpp"

using namespace std;

int main()
{
    auto cfg = mca::Config::fromRaytrixCfgFilePath(R"(D:\code\SIGS\230818_VVC\dataset\config\NagoyaFujita\param.cfg)");
    auto src = cv::imread("src_square.png");
    cv::Mat dst;
    mca::dbg::dbgDrawMicroImageEdges(cfg, src, dst);
    cv::imwrite("dbg_circles_square.png", dst);
    return 0;
}
