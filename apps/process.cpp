#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <opencv2/imgcodecs.hpp>

#include "LVC/config/parser.h"

#include "LVC/preprocess/backward.h"
#include "LVC/preprocess/forward.h"

namespace stdfs = std::filesystem;

int main(int argc, char* argv[])
{
    stdfs::path root = stdfs::path(argv[1]);
    auto src_dir = root / "src";
    auto dst_dir = root / "src_pre";

    auto cfg = lvc::fromRaytrixCfgFilePath((dst_dir / "param_pre.cfg").string());
    for (int i = 1; i < 31; i++) {
        cv::Mat src, dst, backward_src;
        std::stringstream s_filename;
        s_filename << std::setfill('0') << std::setw(3) << i << ".png";
        auto src_path = src_dir / s_filename.str();
        src = cv::imread(src_path.string());
        lvc::preprocessForward(cfg, src, dst);
        lvc::preprocessBackward(cfg, dst, backward_src);
        auto dst_path = dst_dir / s_filename.str();
        cv::imwrite(dst_path.string(), backward_src);
    }
    return 0;
}
