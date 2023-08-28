#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config_reader.h"
#include "LVC/preprocess/debug.h"

using namespace std;

int main()
{
    auto cfg=
        fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230508_LVC\code\rlc-master\TestDataset\ChessPieces\param_pre.cfg)");
    auto src = cv::imread("square_raw.png");
    cv::Mat dst;
    lvc::dbgDrawMicroImageEdges(cfg, src, dst);
    cv::imwrite("debug.png", dst);
    return 0;
}
