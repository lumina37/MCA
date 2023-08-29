#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config_parser.h"
#include "LVC/preprocess/debug.h"

using namespace std;

int main()
{
    auto cfg = fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230818_VVC\dataset\videos\ChessPieces\param.cfg)");
    auto src = cv::imread("square_raw.png");
    cv::Mat dst;
    lvc::dbgDrawMicroImageEdges(cfg, src, dst);
    cv::imwrite("debug.png", dst);
    return 0;
}
