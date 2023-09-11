#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/config/parser.h"
#include "LVC/preprocess/forward.h"

int main()
{
    auto cfg = lvc::fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230818_VVC\dataset\videos\NagoyaFujita\param.cfg)");
    auto src = cv::imread("src_square.png");
    cv::Mat dst;
    lvc::preprocessForward(cfg, src, dst);
    cv::imwrite("forward.png", dst);
    return 0;
}
