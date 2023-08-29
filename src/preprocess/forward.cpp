#include <opencv2/core.hpp>

#include "LVC/common/helper.hpp"
#include "LVC/config/config.h"

#include "LVC/preprocess/forward.h"

namespace lvc {

void preprocessForward(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    double inscribed_square_width = cfg.getDiameter() * cfg.getSquareWidthRatio();
    int inscribed_square_width_i = static_cast<int>(round(inscribed_square_width));

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    dst.create(mis.getNumY() * inscribed_square_width_i, mis.getNumX() * inscribed_square_width_i, src.type());
    cv::Mat src_roi_image, dst_roi_image;

    for (const auto& mi : mis) {
        getRoiImageByCenter(src, src_roi_image, mi.getCenter(), inscribed_square_width);
        getRoiImageByLeftupCorner2i(dst, dst_roi_image, mi.getIndex() * inscribed_square_width_i,
                                    inscribed_square_width);
        src_roi_image.copyTo(dst_roi_image);
    }
}

} // namespace lvc