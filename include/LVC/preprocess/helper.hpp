#pragma once

#include <cmath>

#include <opencv2/core.hpp>

#include "LVC/common/const.h"
#include "LVC/common/defines.h"

namespace lvc {

static LVC_CONSTFUNC LVC_FORCE_INLINE double calcDefaultIntervalHorizontal(double diameter) noexcept
{
    return diameter;
}

static LVC_CONSTFUNC LVC_FORCE_INLINE double calcDefaultIntervalVertical(double diameter) noexcept
{
    return dSQRT3_2 * diameter;
}

static LVC_CONSTFUNC LVC_FORCE_INLINE int calcNumHorizontal(int width, double diameter) noexcept
{
    double num_x = static_cast<double>(width) / diameter - 1.0;
    return static_cast<int>(num_x);
}

static LVC_CONSTFUNC LVC_FORCE_INLINE int calcNumVertical(int height, double diameter) noexcept
{
    double num_y = static_cast<double>(height) / (diameter * dSQRT3_2) - 1.0;
    return static_cast<int>(num_y);
}

static LVC_CONSTFUNC LVC_FORCE_INLINE int calcReservedWidth(double width) noexcept
{
    return static_cast<int>(ceil(width)) + 1;
}

static LVC_FORCE_INLINE void getRoiImageByCenter(const cv::Mat& src, cv::Mat& dst, const cv::Point2d& center,
                                                 double width) noexcept
{
    int leftup_x = static_cast<int>(center.x - width / 2.0);
    int leftup_y = static_cast<int>(center.y - width / 2.0);
    int width_i = calcReservedWidth(width);
    cv::Rect roi(leftup_x, leftup_y, width_i, width_i);
    dst = src(roi);
}

static LVC_FORCE_INLINE void getRoiImageByLeftupCorner(const cv::Mat& src, cv::Mat& dst, const cv::Point2d& corner,
                                                       double width) noexcept
{
    int leftup_x = static_cast<int>(corner.x);
    int leftup_y = static_cast<int>(corner.y);
    int width_i = calcReservedWidth(width);
    cv::Rect roi(leftup_x, leftup_y, width_i, width_i);
    dst = src(roi);
}

static LVC_FORCE_INLINE void getRoiImageByLeftupCorner(const cv::Mat& src, cv::Mat& dst, const cv::Point2i& corner,
                                                       double width) noexcept
{
    int width_i = calcReservedWidth(width);
    cv::Rect roi(corner.x, corner.y, width_i, width_i);
    dst = src(roi);
}

static LVC_FORCE_INLINE void centeredCopyMakeBorder(const cv::Mat& src, cv::Mat& dst, const cv::Point2d& abs_center,
                                                    double src_width, double dst_width) noexcept
{
    int src_width_i = calcReservedWidth(src_width);
    int dst_width_i = calcReservedWidth(dst_width);
    dst.create(dst_width_i, dst_width_i, CV_64FC3);

    double dst_left_abs = floor(abs_center.x - dst_width / 2.0);
    double dst_up_abs = floor(abs_center.y - dst_width / 2.0);
    cv::Point2d center = abs_center - cv::Point2d(dst_left_abs, dst_up_abs);
    double src_left = center.x - src_width / 2.0;
    int src_left_i = static_cast<int>(src_left);
    double src_up = center.y - src_width / 2.0;
    int src_up_i = static_cast<int>(src_up);
    double src_right = center.x + src_width / 2.0;
    int src_right_i = static_cast<int>(ceil(src_right));
    double src_down = center.y + src_width / 2.0;
    int src_down_i = static_cast<int>(ceil(src_down));
    double dst_left = center.x - dst_width / 2.0;
    int dst_left_i = static_cast<int>(dst_left);
    double dst_up = center.y - dst_width / 2.0;
    int dst_up_i = static_cast<int>(dst_up);
    double dst_right = center.x + dst_width / 2.0;
    int dst_right_i = static_cast<int>(ceil(dst_right));
    double dst_down = center.y + dst_width / 2.0;
    int dst_down_i = static_cast<int>(ceil(dst_down));

    // Paste src to dst
    cv::Mat dst_central_area = dst(cv::Rect(src_left_i, src_up_i, src_width_i, src_width_i));
    src.copyTo(dst_central_area);

    /* Up-side */
    // Blend
    double up_blend_inner_weight = src_up - floor(src_up);
    cv::Mat up_inner = dst(cv::Rect(src_left_i, src_up_i + 1, src_width_i, 1));
    cv::Mat up_outer = dst(cv::Rect(src_left_i, src_up_i, src_width_i, 1));
    cv::Mat up_blend_single = up_inner * up_blend_inner_weight + up_outer * (1.0 - up_blend_inner_weight);
    // Replicate
    cv::Mat up_blend_area = dst(cv::Range(dst_up_i, src_up_i + 1), cv::Range(src_left_i, src_left_i + src_width_i));
    cv::repeat(up_blend_single, up_blend_area.rows, 1, up_blend_area);
    // Pre-mult
    double up_premult_factor = ceil(dst_up) - dst_up;
    cv::Mat up_premult_single = up_blend_area(cv::Range(0, 1), cv::Range::all());
    up_premult_single *= up_premult_factor;

    /* Down-side */
    // Blend
    double down_blend_inner_weight = ceil(src_down) - src_down;
    cv::Mat down_inner = dst(cv::Rect(src_left_i, src_down_i - 2, src_width_i, 1));
    cv::Mat down_outer = dst(cv::Rect(src_left_i, src_down_i - 1, src_width_i, 1));
    cv::Mat down_blend_single = down_inner * down_blend_inner_weight + down_outer * (1.0 - down_blend_inner_weight);
    // Replicate
    cv::Mat down_blend_area =
        dst(cv::Range(src_down_i - 1, dst_down_i), cv::Range(src_left_i, src_left_i + src_width_i));
    cv::repeat(down_blend_single, down_blend_area.rows, 1, down_blend_area);
    // Pre-mult
    double down_premult_factor = dst_down - floor(dst_down);
    cv::Mat down_premult_single =
        down_blend_area(cv::Range(down_blend_area.rows - 1, down_blend_area.rows), cv::Range::all());
    down_premult_single *= down_premult_factor;

    /* Left-side */
    // Blend
    double left_blend_inner_weight = src_left - floor(src_left);
    cv::Mat left_inner = dst(cv::Rect(src_left_i + 1, src_up_i, 1, src_width_i));
    cv::Mat left_outer = dst(cv::Rect(src_left_i, src_up_i, 1, src_width_i));
    cv::Mat left_blend_single = left_inner * left_blend_inner_weight + left_outer * (1.0 - left_blend_inner_weight);
    // Replicate
    cv::Mat left_blend_area = dst(cv::Range(src_up_i, src_up_i + src_width_i), cv::Range(dst_left_i, src_left_i + 1));
    cv::repeat(left_blend_single, 1, left_blend_area.cols, left_blend_area);
    // Pre-mult
    double left_premult_factor = ceil(dst_left) - dst_left;
    cv::Mat left_premult_single = left_blend_area(cv::Range::all(), cv::Range(0, 1));
    left_premult_single *= left_premult_factor;

    /* Right-side */
    // Blend
    double right_blend_inner_weight = ceil(src_right) - src_right;
    cv::Mat right_inner = dst(cv::Rect(src_right_i - 2, src_up_i, 1, src_width_i));
    cv::Mat right_outer = dst(cv::Rect(src_right_i - 1, src_up_i, 1, src_width_i));
    cv::Mat right_blend_single =
        right_inner * right_blend_inner_weight + right_outer * (1.0 - right_blend_inner_weight);
    // Replicate
    cv::Mat right_blend_area =
        dst(cv::Range(src_up_i, src_up_i + src_width_i), cv::Range(src_right_i - 1, dst_right_i));
    cv::repeat(right_blend_single, 1, right_blend_area.cols, right_blend_area);
    // Pre-mult
    double right_premult_factor = dst_right - floor(dst_right);
    cv::Mat right_premult_single =
        right_blend_area(cv::Range::all(), cv::Range(right_blend_area.cols - 1, right_blend_area.cols));
    right_premult_single *= right_premult_factor;
}

} // namespace lvc