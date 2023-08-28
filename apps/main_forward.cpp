#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config_reader.h"
#include "LVC/preprocess/forward.h"

int main()
{
    auto cfg=
        fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230508_LVC\code\rlc-master\TestDataset\ChessPieces\param_pre.cfg)");
    auto src = cv::imread("raw.png");
    cv::Mat dst;
    lvc::preprocessForward(cfg, src, dst);
    cv::imwrite("forward.png", dst);
    return 0;
}
