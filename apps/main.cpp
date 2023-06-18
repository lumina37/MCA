#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/preprocess/debug.hpp"

using namespace std;

int main()
{
    lvc::Config cfg(23.202295303345f, 2048, 2048, 12.2f, 12.125f, true, false);
    auto src = cv::imread("raw.png");
    cv::Mat dst;
    lvc::dbgDrawMicroImageEdges(cfg, src, dst);
    cv::imwrite("debug.png", dst);
    return 0;
}