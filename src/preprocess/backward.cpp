#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "LVC/common/defines.h"
#include "LVC/config/config.h"
#include "LVC/preprocess/helper.hpp"

#include "LVC/preprocess/backward.h"

namespace lvc {

static LVC_FORCE_INLINE void genCircleMask(cv::Mat& dst, double diameter)
{
    cv::circle(dst, {dst.cols / 2, dst.rows / 2}, static_cast<int>(diameter / 2.0), cv::Scalar::all(255.0),
               cv::LineTypes::FILLED, cv::LineTypes::LINE_AA);
}

void preprocessBackward(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    dst = cv::Mat::zeros(cfg.getHeight(), cfg.getWidth(), src.type());

    double inscribed_square_width = cfg.getDiameter() * cfg.getSquareWidthRatio();
    int inscribed_square_width_i = static_cast<int>(ceil(inscribed_square_width));
    int diameter_i = static_cast<int>(ceil(cfg.getDiameter()));

    cv::Mat src_roi_image, dst_roi_image, src_roi_image_with_border;
    cv::Mat mask_image = cv::Mat::zeros(diameter_i, diameter_i, CV_8U);
    genCircleMask(mask_image, cfg.getDiameter());

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    for (const auto& mi : mis) {
        cv::Point2d mi_center = mi.getCenter();

        getRoiImageByLeftupCorner2i(src, src_roi_image, mi.getIndex() * inscribed_square_width_i,
                                    inscribed_square_width);
        getRoiImageByCenter(dst, dst_roi_image, mi_center, cfg.getDiameter());

        int dst_leftup_corner_x = static_cast<int>(mi_center.x - cfg.getDiameter() / 2.0);
        int dst_leftup_corner_y = static_cast<int>(mi_center.y - cfg.getDiameter() / 2.0);
        int src_leftup_corner_x = static_cast<int>(mi_center.x - inscribed_square_width / 2.0);
        int src_leftup_corner_y = static_cast<int>(mi_center.y - inscribed_square_width / 2.0);
        int left_border_width = src_leftup_corner_x - dst_leftup_corner_x;
        int top_border_width = src_leftup_corner_y - dst_leftup_corner_y;
        int right_border_width = diameter_i - inscribed_square_width_i - left_border_width;
        int bot_border_width = diameter_i - inscribed_square_width_i - top_border_width;
        cv::copyMakeBorder(src_roi_image, src_roi_image_with_border, top_border_width, bot_border_width,
                           left_border_width, right_border_width,
                           cv::BorderTypes::BORDER_REPLICATE | cv::BorderTypes::BORDER_ISOLATED);

        cv::add(src_roi_image_with_border, dst_roi_image, dst_roi_image, mask_image);
    }
}

} // namespace lvc