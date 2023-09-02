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
    std::string cfg_path_str = std::string(argv[1]);
    auto cfg_path = stdfs::path(cfg_path_str);
    auto src_dir = stdfs::path(argv[2]);
    auto dst_dir = stdfs::path(argv[3]);

    auto cfg = lvc::fromRaytrixCfgFilePath(cfg_path_str);
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
