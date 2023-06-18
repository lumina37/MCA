#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/preprocess/debug.hpp"

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
    lvc::Config cfg(23.202295303345f, 2048, 2048, 12.2f, 12.125f, true, false);
    std::vector<cv::Mat> maps = {cv::imread("patchSizeMap_0.png", cv::IMREAD_GRAYSCALE),
                                 cv::imread("patchSizeMap_1.png", cv::IMREAD_GRAYSCALE),
                                 cv::imread("patchSizeMap_2.png", cv::IMREAD_GRAYSCALE)};

    cv::Mat patch_size_map;
    mergePatchSizeMap(maps, {{1, 0, 2}, {2, 1, 0}}, patch_size_map);

    if (!cfg.getIsHorizontal()) {
        cv::Mat temp = patch_size_map;
        cv::transpose(temp, patch_size_map);
    }

    cv::Mat dst;
    lvc::dbgDrawUsedArea(cfg, patch_size_map, 3, dst);
    cv::imwrite("used_area.png", dst);

    return 0;
}
