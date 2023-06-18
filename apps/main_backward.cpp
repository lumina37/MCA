#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/preprocess/backward.hpp"

using namespace std;

int main()
{
    lvc::Config cfg(34.976657867432f, 3840, 2160, 26.7233f, 27.65f, true);
    auto src = cv::imread("forward.png");
    cv::Mat dst;
    lvc::preprocessBackward(cfg, src, dst);
    cv::imwrite("forward_backward.png", dst);
    return 0;
}
