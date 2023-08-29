#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <opencv2/imgcodecs.hpp>

#include "LVC/config/parser.h"

#include "LVC/preprocess/backward.h"
#include "LVC/preprocess/forward.h"

int main(int argc, char* argv[])
{
    std::string root = argv[1];
    std::string src_dir = root + "/src/";
    std::string dst_dir = root + "/src_pre/";

    auto cfg = fromRaytrixCfgFilePath(root + "/param_pre.cfg");
    for (int i = 1; i < 31; i++) {
        cv::Mat src, dst, backward_src;
        std::stringstream s_filename;
        s_filename << std::setfill('0') << std::setw(3) << i << ".png";
        std::string src_path = src_dir + s_filename.str();
        src = cv::imread(src_path);
        lvc::preprocessForward(cfg, src, dst);
        lvc::preprocessBackward(cfg, dst, backward_src);
        std::string dst_path = dst_dir + s_filename.str();
        cv::imwrite(dst_path, backward_src);
    }
    return 0;
}
