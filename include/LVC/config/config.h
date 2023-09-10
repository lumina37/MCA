#pragma once

#include <opencv2/core.hpp>

#include "LVC/common/defines.h"
#include "LVC/preprocess/helper.hpp"

namespace lvc {

class LVC_EXPORT Config
{
public:
    /**
     * @brief 存储一切配置参数
     *
     * @param diameter Micro Image的直径
     * @param width 原始图像的宽度
     * @param height 原始图像的高度
     * @param start_x 第一行最靠左的MI(即最左上角的MI)的x坐标
     * @param start_y 第一行最靠左的MI的y坐标
     * @param is_positive_shift {
     * `true`则第二个密排行/列位于第一个密排行/列的右侧/下侧(例如chess和NagoyaFujita序列)
     * `false`则第二个密排行/列位于第一个密排行/列的左侧/上侧
     * 默认为`true`
     * }
     * @param is_tight_row {
     * `true`则MI横向密排而纵向左右交错(例如chess序列)
     * `false`则MI纵向密排而横向上下交错(NagoyaFujita)
     * 默认为`true`
     * }
     * @param interval_x {
     * 若`is_tight_row`为`true`则为横向两个直排MI的间距 反之为纵向两个直排MI的间距
     * 默认为 $2*diameter$
     * }
     * @param interval_y {
     * 若`is_tight_row`为`true`则为纵向两个交错MI的间距 反之为横向两个交错MI的间距
     * 默认为 $\\sqrt{3}/2*diameter$
     * }
     * @param square_width_diam_ratio 裁切正方形宽度与直径的比值
     *
     * @note x对应width对应columns y对应height对应rows
     */
    Config(double diameter, int width, int height, double start_x, double start_y, bool is_positive_shift = true,
           bool is_tight_row = true, double interval_x = 0.0, double interval_y = 0.0,
           double square_width_diam_ratio = d1_SQRT2) noexcept;

    double getDiameter() const noexcept { return diameter_; };
    int getWidth() const noexcept { return width_; };
    int getHeight() const noexcept { return height_; };
    double getStartX() const noexcept { return start_x_; };
    double getStartY() const noexcept { return start_y_; };
    double getIntervalX() const noexcept { return interval_x_; };
    double getIntervalY() const noexcept { return interval_y_; };
    double getSquareWidthRatio() const noexcept { return square_width_diam_ratio_; };
    bool getIsPositiveShift() const noexcept { return is_positive_shift_; };
    bool getIsTightRow() const noexcept { return is_tight_row_; };

private:
    double diameter_;
    int width_;
    int height_;
    double start_x_;
    double start_y_;
    double interval_x_;
    double interval_y_;
    double square_width_diam_ratio_;
    bool is_tight_row_;
    bool is_positive_shift_;
};

class LVC_EXPORT MicroImage
{
public:
    /**
     * @brief 用于对MI定位
     *
     * @param x MI的x坐标
     * @param y MI的y坐标
     * @param index_x MI的x轴索引值
     * @param index_y MI的y轴索引值
     *
     * @note x对应width对应columns y对应height对应rows
     */
    MicroImage(double x, double y, int index_x, int index_y) noexcept;

    cv::Point2d getCenter() const noexcept { return {x_, y_}; };
    cv::Point2i getIndex() const noexcept { return {index_x_, index_y_}; };

    const double x_;
    const double y_;
    const int index_x_;
    const int index_y_;
};

class LVC_EXPORT MicroImageRanges
{
public:
    /**
     * @brief 用于遍历MI 支持标准`for range`语法
     *
     * @param start_x 第一行最靠左的MI(即最左上角的MI)的x坐标
     * @param start_y 第一行最靠左的MI的y坐标
     * @param interval_x 若`is_tight_row`为`true`则为横向两个直排MI的间距 反之为纵向两个直排MI的间距
     * @param interval_y 若`is_tight_row`为`true`则为纵向两个交错MI的间距 反之为横向两个交错MI的间距
     * @param num_x x轴方向上的MI索引数量
     * @param num_y y轴方向上的MI索引数量
     * @param is_positive_shift {
     * `true`则第二个密排行/列位于第一个密排行/列的右侧/下侧(例如chess和NagoyaFujita序列)
     * `false`则第二个密排行/列位于第一个密排行/列的左侧/上侧
     * 默认为`true`
     * }
     * @param is_tight_row {
     * `true`则MI横向密排而纵向左右交错(例如chess序列)
     * `false`则MI纵向密排而横向上下交错(NagoyaFujita)
     * 默认为`true`
     * }
     */
    MicroImageRanges(double start_x, double start_y, double interval_x, double interval_y, int num_x, int num_y,
                     bool is_positive_shift, bool is_tight_row) noexcept;

    class iterator;

    /**
     * @brief 从已有的`Config`生成用于遍历MI的`MicroImageRanges`
     * @param cfg 感觉不用解释
     * @return MicroImageRanges
     */
    static MicroImageRanges fromConfig(const Config& cfg) noexcept;

    iterator begin() const noexcept;
    iterator end() const noexcept;

    int getNumX() const noexcept { return num_x_; }
    int getNumY() const noexcept { return num_y_; }

private:
    double start_x_;
    double start_y_;
    double interval_x_;
    double interval_y_;
    int num_x_;
    int num_y_;
    bool is_tight_row_;
    bool is_positive_shift_;
};

class MicroImageRanges::iterator
{
public:
    iterator(double start_x, double start_y, double interval_x, double interval_y, int index_x, int index_y, int num_x,
             int num_y, bool is_tight_row, bool is_positive_shift) noexcept;

    MicroImage fromIndex(int index_x, int index_y) const noexcept;

    iterator& operator++() noexcept;
    bool operator!=(const iterator& rhs) const noexcept { return index_x_ != rhs.index_x_ || index_y_ != rhs.index_y_; }
    MicroImage operator*() const noexcept { return fromIndex(index_x_, index_y_); }

private:
    double start_x_;
    double start_y_;
    double interval_x_;
    double interval_y_;
    int index_x_;
    int index_y_;
    int num_x_;
    int num_y_;
    bool is_tight_row_;
    bool is_positive_shift_;
};

} // namespace lvc