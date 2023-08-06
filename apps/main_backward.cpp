#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/preprocess/backward.hpp"

#include "__rlc_config.hpp"

using namespace std;

int main()
{
    auto cfg= fromRlcCfgFilePath(R"(D:\Code\SIGS\230508_LVC\code\rlc-master\TestDataset\ChessPieces\param_pre.cfg)");
    auto src = cv::imread("forward.png");
    cv::Mat dst;
    lvc::preprocessBackward(cfg, src, dst);
    cv::imwrite("forward_backward.png", dst);
    return 0;
}
