#include <opencv2/core.hpp>

#include "LVC/config/config.h"
#include "LVC/preprocess/helper.hpp"

#include "LVC/preprocess/forward.h"

namespace lvc {

void preprocessForward(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    cv::Mat src_f, dst_f;
    src.convertTo(src_f, CV_32FC3);
    double inscribed_square_width = cfg.getDiameter() * cfg.getSquareWidthRatio();
    int inscribed_square_width_i = static_cast<int>(ceil(inscribed_square_width));

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    dst_f.create(mis.getNumY() * inscribed_square_width_i, mis.getNumX() * inscribed_square_width_i, src_f.type());
    cv::Mat src_roi_image, dst_roi_image;

    for (const auto& mi : mis) {
        getRoiImageByCenter(src_f, src_roi_image, mi.getCenter(), inscribed_square_width);
        edgePreMult_(src_roi_image, mi.getCenter());
        getRoiImageByLeftupCorner2i(dst_f, dst_roi_image, mi.getIndex() * inscribed_square_width_i,
                                    inscribed_square_width);
        src_roi_image.copyTo(dst_roi_image);
    }

    dst_f.convertTo(dst, CV_8UC3);
}

} // namespace lvc