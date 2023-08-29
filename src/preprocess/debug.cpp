#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "LVC/common/defines.h"
#include "LVC/config/config.h"

#include "LVC/preprocess/debug.h"

namespace lvc {

static LVC_FORCE_INLINE void dbgDrawCircle(cv::Mat& dst, cv::Point2d center, double diameter)
{
    cv::circle(dst, static_cast<cv::Point2i>(center), static_cast<int>(diameter / 2.0), cv::Scalar(0, 255, 0), 1,
               cv::LineTypes::LINE_AA);
}

static LVC_FORCE_INLINE void dbgDrawSoildCircle(cv::Mat& dst, cv::Point2d center, double diameter, cv::Scalar color)
{
    cv::circle(dst, static_cast<cv::Point2i>(center), static_cast<int>(diameter / 2.0), color, cv::LineTypes::FILLED,
               cv::LineTypes::LINE_AA);
}

void dbgDrawMicroImageEdges(const Config& cfg, const cv::Mat& src, cv::Mat& dst)
{
    dst = src.clone();

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);

    for (const auto& mi : mis) {
        dbgDrawCircle(dst, mi.getCenter(), cfg.getDiameter());
    }
}

void dbgDrawUsedArea(const Config& cfg, const cv::Mat& patch_size_map, int view_num, cv::Mat& dst)
{
    dst = cv::Mat::zeros(cfg.getHeight(), cfg.getWidth(), CV_8UC3);

    MicroImageRanges mis = MicroImageRanges::fromConfig(cfg);
    int view_shift = view_num / 2;

    for (const auto& mi : mis) {
        auto idx = mi.getIndex();
        if (idx.x >= patch_size_map.cols || idx.y >= patch_size_map.rows) {
            continue;
        }

        double patch_size = static_cast<double>(patch_size_map.at<uchar>(idx.y, idx.x));
        cv::Scalar color;
        if (patch_size + static_cast<double>(view_num) > cfg.getDiameter()) {
            color = cv::Scalar(0, 0, 160);
        } else {
            color = cv::Scalar::all(255);
        }

        dbgDrawCircle(dst, mi.getCenter(), cfg.getDiameter());

        for (int view_shift_x = -view_shift; view_shift_x <= view_shift; view_shift_x++) {
            for (int view_shift_y = -view_shift; view_shift_y <= view_shift; view_shift_y++) {
                cv::Point2i shift = {view_shift_x, view_shift_y};
                dbgDrawSoildCircle(dst, mi.getCenter() + static_cast<cv::Point2d>(shift), patch_size, color);
            }
        }
    }
}

} // namespace lvc
