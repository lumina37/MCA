#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "mca/config/parser.h"
#include "mca/process/debug.h"

using namespace std;

int main()
{
    auto cfg = mca::fromRaytrixCfgFilePath(R"(D:\code\SIGS\230818_VVC\dataset\config\NagoyaFujita\param.cfg)");
    auto src = cv::imread("src_square.png");
    cv::Mat dst;
    mca::dbgDrawMicroImageEdges(cfg, src, dst);
    cv::imwrite("dbg_circles_square.png", dst);
    return 0;
}
