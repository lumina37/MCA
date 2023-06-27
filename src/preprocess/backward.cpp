#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "LVC/common/config.hpp"
#include "LVC/common/defines.h"
#include "LVC/common/helper.hpp"

#include "LVC/preprocess/backward.hpp"

namespace lvc {

static LVC_FORCE_INLINE void genCircleMask(cv::Mat& dst, float diameter)
{
    cv::circle(dst, {dst.cols / 2, dst.rows / 2}, static_cast<int>(diameter / 2.0f), cv::Scalar::all(255.0),
               cv::LineTypes::FILLED, cv::LineTypes::LINE_AA);
}

void preprocessBackward(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    dst.create(cfg.getHeight(), cfg.getWidth(), src.type());

    float inscribed_square_width = cfg.getDiameter() * cfg.getSquareWidthRatio();
    int inscribed_square_width_i = static_cast<int>(round(inscribed_square_width));
    int diameter_i = static_cast<int>(round(cfg.getDiameter()));
    int lefttop_border_width = (diameter_i - inscribed_square_width_i) / 2;
    int rightbot_border_width = diameter_i - inscribed_square_width_i - lefttop_border_width;

    cv::Mat src_roi_image, dst_roi_image, dst_roi_image_with_border;
    cv::Mat mask_image = cv::Mat::zeros(diameter_i, diameter_i, src.type());
    genCircleMask(mask_image, cfg.getDiameter());

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    for (const auto& mi : mis) {
        getRoiImageByLeftupCorner2i(src, src_roi_image, mi.getIndex() * inscribed_square_width_i,
                                    inscribed_square_width);
        cv::copyMakeBorder(src_roi_image, dst_roi_image_with_border, lefttop_border_width, rightbot_border_width,
                           lefttop_border_width, rightbot_border_width, cv::BorderTypes::BORDER_REFLECT);
        getRoiImageByCenter(dst, dst_roi_image, mi.getCenter(), cfg.getDiameter());
        dst_roi_image_with_border.copyTo(dst_roi_image, mask_image);
    }
}

} // namespace lvc