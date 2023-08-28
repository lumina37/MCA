#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/common/config_reader.h"
#include "LVC/preprocess/backward.h"

using namespace std;

int main()
{
    auto cfg=
        fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230508_LVC\code\rlc-master\TestDataset\ChessPieces\param_pre.cfg)");
    auto src = cv::imread("forward.png");
    cv::Mat dst;
    lvc::preprocessBackward(cfg, src, dst);
    cv::imwrite("forward_backward.png", dst);
    return 0;
}
