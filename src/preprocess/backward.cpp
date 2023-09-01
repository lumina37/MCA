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
    cv::Mat src_f;
    src.convertTo(src_f, CV_64FC3);
    cv::Mat dst_f = cv::Mat::zeros(cfg.getHeight(), cfg.getWidth(), src_f.type());

    double src_block_width = cfg.getDiameter() * cfg.getSquareWidthRatio();
    int src_block_width_i = calcReservedWidth(src_block_width);
    int dst_block_width_i = calcReservedWidth(cfg.getDiameter());

    cv::Mat src_roi_image, dst_roi_image, src_roi_image_with_border;
    cv::Mat mask_image = cv::Mat::zeros(dst_block_width_i, dst_block_width_i, CV_8U);
    genCircleMask(mask_image, cfg.getDiameter());

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    for (const auto& mi : mis) {
        cv::Point2d mi_center = mi.getCenter();

        getRoiImageByLeftupCorner(src_f, src_roi_image, mi.getIndex() * src_block_width_i, src_block_width);
        getRoiImageByCenter(dst_f, dst_roi_image, mi_center, cfg.getDiameter());
        centeredCopyMakeBorder(src_roi_image, src_roi_image_with_border, mi_center, src_block_width, cfg.getDiameter());

        cv::add(src_roi_image_with_border, dst_roi_image, dst_roi_image, mask_image);
    }

    dst_f.convertTo(dst, CV_8UC3);
}

} // namespace lvc