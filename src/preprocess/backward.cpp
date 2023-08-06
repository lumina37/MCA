#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "LVC/common/config.hpp"
#include "LVC/common/defines.h"
#include "LVC/common/helper.hpp"

#include "LVC/preprocess/backward.hpp"

namespace lvc {

static LVC_FORCE_INLINE void genCircleMask(cv::Mat& dst, double diameter)
{
    cv::circle(dst, {dst.cols / 2, dst.rows / 2}, static_cast<int>(diameter / 2.0), cv::Scalar::all(255.0),
               cv::LineTypes::FILLED, cv::LineTypes::LINE_AA);
}

void preprocessBackward(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    dst.create(cfg.getHeight(), cfg.getWidth(), src.type());

    double inscribed_square_width = cfg.getDiameter() * cfg.getSquareWidthRatio();
    int inscribed_square_width_i = static_cast<int>(round(inscribed_square_width));
    int diameter_i = static_cast<int>(round(cfg.getDiameter()));

    cv::Mat src_roi_image, dst_roi_image, src_roi_image_with_border;
    cv::Mat mask_image = cv::Mat::zeros(diameter_i, diameter_i, src.type());
    genCircleMask(mask_image, cfg.getDiameter());

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    for (const auto& mi : mis) {
        cv::Point2d mi_center = mi.getCenter();

        getRoiImageByLeftupCorner2i(src, src_roi_image, mi.getIndex() * inscribed_square_width_i,
                                    inscribed_square_width);
        getRoiImageByCenter(dst, dst_roi_image, mi_center, cfg.getDiameter());

        int dst_leftup_corner_x = static_cast<int>(round(mi_center.x - cfg.getDiameter() / 2.0));
        int dst_leftup_corner_y = static_cast<int>(round(mi_center.y - cfg.getDiameter() / 2.0));
        int src_leftup_corner_x = static_cast<int>(round(mi_center.x - inscribed_square_width / 2.0));
        int src_leftup_corner_y = static_cast<int>(round(mi_center.y - inscribed_square_width / 2.0));
        int left_border_width = src_leftup_corner_x - dst_leftup_corner_x;
        int top_border_width = src_leftup_corner_y - dst_leftup_corner_y;
        int right_border_width = diameter_i - inscribed_square_width_i - left_border_width;
        int bot_border_width = diameter_i - inscribed_square_width_i - top_border_width;
        cv::copyMakeBorder(src_roi_image, src_roi_image_with_border, top_border_width, bot_border_width,
                           left_border_width, right_border_width,
                           cv::BorderTypes::BORDER_REPLICATE | cv::BorderTypes::BORDER_ISOLATED);

        src_roi_image_with_border.copyTo(dst_roi_image, mask_image);
    }
}

} // namespace lvc