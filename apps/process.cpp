#include <cstdio>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/preprocess/backward.hpp"
#include "LVC/preprocess/forward.hpp"

using namespace std;

enum CameraType { r3840x2160_chess_boxer = 0, r3840x2160_chesspieces, r2048x2048 };

int main()
{
    lvc::Config cfg = {34.976657867432f, 3840, 2160, 26.7233f, 27.65f};
    CameraType type = CameraType::r2048x2048;
    switch (type) {
    case CameraType::r3840x2160_chess_boxer:
        cfg = {34.976657867432f, 3840, 2160, 26.7233f, 27.65f, true};
        break;
    case CameraType::r3840x2160_chesspieces:
        cfg = {34.976657867432f, 3840, 2160, 25.325f, 27.65f, true};
        break;
    case CameraType::r2048x2048:
        cfg = {23.202295303345f, 2048, 2048, 12.2f, 12.125f, true, false};
        break;
    }

    char buf[256];

    // for (int i = 1; i < 31; i++) {
    //     cv::Mat src, dst, backward_src;
    //     snprintf(buf, 256, "D:\\Code\\SIGS\\230508_LVC\\code\\rlc-master\\TestDataset\\Tunnel_Train\\src\\%03d.png", i);  // snprintf 用于构建字符串
    //     src = cv::imread(buf);
    //     lvc::preprocessForward(cfg, src, dst);
    //     lvc::preprocessBackward(cfg, dst, backward_src);
    //     snprintf(buf, 256, "D:\\Code\\SIGS\\230508_LVC\\code\\rlc-master\\TestDataset\\Tunnel_Train\\src_pre\\%03d.png",
    //              i);
    //     cv::imwrite(buf, backward_src);
    // }

    cv::Mat src, dst, backward_src;
    char inputPath[256] = "/mnt/e/Projects/task2-raytrix/libLVC/data/RawImages/NagoyaFujita.png";
    char outputPath[256] = "/mnt/e/Projects/task2-raytrix/libLVC/data/PreprocessedImages/NagoyaFujita.png";
    src = cv::imread(inputPath);
    lvc::preprocessForward(cfg, src, dst);
    lvc::preprocessBackward(cfg, dst, backward_src);
    cv::imwrite(outputPath, backward_src);

    return 0;
}
