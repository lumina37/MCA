#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/preprocess/backward.hpp"

using namespace std;

int main()
{
    lvc::Config cfg(23.202295303345f, 2048, 2048, 12.2f, 12.125f, true, false, 0.0, 0.0, 0.99);
    auto src = cv::imread("forward.png");
    cv::Mat dst;
    lvc::preprocessBackward(cfg, src, dst);
    cv::imwrite("forward_backward.png", dst);
    return 0;
}
