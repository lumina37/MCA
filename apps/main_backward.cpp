#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/config/parser.h"
#include "LVC/preprocess/backward.h"

using namespace std;

int main()
{
    auto cfg = fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230818_VVC\dataset\videos\ChessPieces\param.cfg)");
    auto src = cv::imread("forward.png");
    cv::Mat dst;
    lvc::preprocessBackward(cfg, src, dst);
    cv::imwrite("forward_backward.png", dst);
    return 0;
}
