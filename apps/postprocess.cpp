#include <filesystem>
#include <iostream>
#include <regex>

#include <opencv2/imgcodecs.hpp>

#include "LVC/config/parser.h"

#include "LVC/preprocess/backward.h"

namespace stdfs = std::filesystem;

int main(int argc, char* argv[])
{
    std::string cfg_path_str = std::string(argv[1]);
    auto cfg_path = stdfs::path(cfg_path_str);
    auto src_dir = stdfs::path(argv[2]);
    auto dst_dir = stdfs::path(argv[3]);

    auto cfg = lvc::fromRaytrixCfgFilePath(cfg_path_str);
    std::regex suffix(R"(.*png)");

    for (auto& src_path_iter : stdfs::directory_iterator(src_dir)) {
        const auto& src_path = src_path_iter.path();
        auto src_path_str = src_path.string();

        if (!std::regex_match(src_path_str, suffix)) {
            continue;
        }

        cv::Mat src, dst;
        src = cv::imread(src_path_str);
        lvc::preprocessBackward(cfg, src, dst);
        auto dst_path = dst_dir / src_path.filename();
        cv::imwrite(dst_path.string(), dst);
    }

    return 0;
}
