#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "mca/config/config.h"

#include "mca/process/postproc.h"

namespace mca {

static inline void genCircleMask(cv::Mat& dst, double diameter)
{
    cv::circle(dst, {dst.cols / 2, dst.rows / 2}, static_cast<int>(diameter / 2.0), cv::Scalar::all(255.0),
               cv::LineTypes::FILLED, cv::LineTypes::LINE_AA);
}

void postprocess(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    dst = cv::Mat::zeros(cfg.getHeight(), cfg.getWidth(), src.type());

    double src_block_width = cfg.getDiameter() * cfg.getCropRatio();
    int src_block_width_i = static_cast<int>(round(src_block_width));
    int dst_block_width_i = static_cast<int>(round(cfg.getDiameter()));

    cv::Mat src_roi_image, dst_roi_image, src_roi_image_with_border;
    cv::Mat mask_image = cv::Mat::zeros(dst_block_width_i, dst_block_width_i, src.type());
    genCircleMask(mask_image, cfg.getDiameter());

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    for (const auto& mi : mis) {
        cv::Point2d mi_center = mi.getCenter();

        getRoiImageByLeftupCorner(src, src_roi_image, mi.getIndex() * src_block_width_i, src_block_width);
        getRoiImageByCenter(dst, dst_roi_image, mi_center, cfg.getDiameter());

        int dst_leftup_corner_x = static_cast<int>(round(mi_center.x - cfg.getDiameter() / 2.0));
        int dst_leftup_corner_y = static_cast<int>(round(mi_center.y - cfg.getDiameter() / 2.0));
        int src_leftup_corner_x = static_cast<int>(round(mi_center.x - src_block_width / 2.0));
        int src_leftup_corner_y = static_cast<int>(round(mi_center.y - src_block_width / 2.0));

        int left_border_width = src_leftup_corner_x - dst_leftup_corner_x;
        int top_border_width = src_leftup_corner_y - dst_leftup_corner_y;
        int right_border_width = dst_block_width_i - src_block_width_i - left_border_width;
        int bot_border_width = dst_block_width_i - src_block_width_i - top_border_width;

        cv::copyMakeBorder(src_roi_image, src_roi_image_with_border, top_border_width, bot_border_width,
                           left_border_width, right_border_width,
                           cv::BorderTypes::BORDER_REPLICATE | cv::BorderTypes::BORDER_ISOLATED);

        src_roi_image_with_border.copyTo(dst_roi_image, mask_image);
    }
}

} // namespace mca