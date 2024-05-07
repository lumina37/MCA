#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "mca/config.hpp"
#include "mca/impl/debug.hpp"

static void mergePatchSizeMap(const std::vector<cv::Mat>& maps, std::vector<std::vector<int>> types, cv::Mat& dst)
{
    assert(maps[0].type() == CV_8UC1);
    dst.create(maps[0].rows, maps[0].cols * 3, maps[0].type());
    for (int i = 0; i < dst.rows; i++) {
        auto p_row = dst.ptr<uchar>(i);
        int is_row_odd = i & 1;
        for (int j = 0; j < dst.cols; j++) {
            int type = types[is_row_odd][j % 3];
            *p_row = maps[type].at<uchar>(i, j / 3);
            p_row++;
        }
    }
}

int main()
{
    auto cfg = mca::Config::fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230818_VVC\dataset\videos\ChessPieces\param.cfg)");
    std::vector<cv::Mat> maps{cv::imread("patchSizeMap_0.png", cv::IMREAD_GRAYSCALE),
                              cv::imread("patchSizeMap_1.png", cv::IMREAD_GRAYSCALE),
                              cv::imread("patchSizeMap_2.png", cv::IMREAD_GRAYSCALE)};

    cv::Mat patch_size_map;
    mergePatchSizeMap(maps, {{1, 0, 2}, {2, 1, 0}}, patch_size_map);

    if (false) {
        cv::Mat temp = patch_size_map;
        cv::transpose(temp, patch_size_map);
    }

    cv::Mat dst_3, dst_5;
    mca::dbgDrawUsedArea(cfg, patch_size_map, 3, dst_3);
    cv::imwrite("used_area_3.png", dst_3);
    mca::dbgDrawUsedArea(cfg, patch_size_map, 5, dst_5);
    cv::imwrite("used_area_5.png", dst_5);

    return 0;
}
