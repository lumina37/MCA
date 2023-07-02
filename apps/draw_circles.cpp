#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/preprocess/debug.hpp"

#include "__rlc_config.hpp"

using namespace std;

int main()
{
    auto cfg= fromRlcCfgFilePath(R"(D:\Code\SIGS\230508_LVC\code\rlc-master\TestDataset\NagoyaFujita\param_pre.cfg)");
    auto src = cv::imread("square_raw.png");
    cv::Mat dst;
    lvc::dbgDrawMicroImageEdges(cfg, src, dst);
    cv::imwrite("debug.png", dst);
    return 0;
}
