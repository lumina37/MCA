#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "MCA/config/parser.h"
#include "MCA/process/postproc.h"

using namespace std;

int main()
{
    auto cfg = mca::fromRaytrixCfgFilePath(R"(D:\code\SIGS\230818_VVC\dataset\config\ChessPieces\param.cfg)");
    auto src = cv::imread("forward.png");
    cv::Mat dst;
    mca::postprocess(cfg, src, dst);
    cv::imwrite("forward_backward.png", dst);
    return 0;
}
