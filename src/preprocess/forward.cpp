#include <opencv2/core.hpp>

#include "LVC/config/config.h"

#include "LVC/preprocess/forward.h"

namespace lvc {

void preprocessForward(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    double block_width = cfg.getDiameter() * cfg.getSquareWidthRatio();
    int block_width_i = static_cast<int>(round(block_width));

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    auto align = [](int num) {
        constexpr int target = 2;
        int pad = (target - num % target) % target;
        return num + pad;
    };

    dst.create(align(mis.getMaxNumY() * block_width_i), align(mis.getMaxNumX() * block_width_i), src.type());
    cv::Mat src_roi_image, dst_roi_image;

    for (const auto& mi : mis) {
        getRoiImageByCenter(src, src_roi_image, mi.getCenter(), block_width);
        getRoiImageByLeftupCorner(dst, dst_roi_image, mi.getIndex() * block_width_i, block_width);
        src_roi_image.copyTo(dst_roi_image);
    }
}

} // namespace lvc