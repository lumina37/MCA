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
     * @param heads 第x行最靠左的MI的坐标点组成的向量
     * @param square_width_diam_ratio 裁切正方形宽度与直径的比值
     *
     * @note x对应width对应columns y对应height对应rows
     */
    Config(double diameter, int width, int height, const cv::Vec<cv::Point2d, 2>& heads,
           double square_width_diam_ratio = d1_SQRT2) noexcept;

    double getDiameter() const noexcept { return diameter_; };
    int getWidth() const noexcept { return width_; };
    int getHeight() const noexcept { return height_; };
    const cv::Vec<cv::Point2d, 2> getHeads() const noexcept { return heads_; };
    double getSquareWidthRatio() const noexcept { return square_width_diam_ratio_; };

private:
    double diameter_;
    int width_;
    int height_;
    cv::Vec<cv::Point2d, 2> heads_;
    double square_width_diam_ratio_;
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
     * @param heads 第x行最靠左的MI的坐标点组成的向量
     * @param vec_num_x 各行x轴方向上的MI索引数量组成的向量
     * @param num_y y轴方向上的MI索引数量
     */
    MicroImageRanges(const cv::Vec<cv::Point2d, 2>& heads, double interval_x, double interval_y,
                     const cv::Vec<int, 2>& vec_num_x, int num_y) noexcept;

    class iterator;

    /**
     * @brief 从已有的`Config`生成用于遍历MI的`MicroImageRanges`
     * @param cfg 感觉不用解释
     * @return MicroImageRanges
     */
    static MicroImageRanges fromConfig(const Config& cfg) noexcept;

    iterator begin() const noexcept;
    iterator end() const noexcept;

    double getIntervalX() const noexcept { return interval_x_; }
    double getIntervalY() const noexcept { return interval_y_; }
    int getNumXi(int row) const noexcept { return vec_num_x_[row]; }
    int getMaxNumX() const noexcept { return vec_num_x_[0] > vec_num_x_[1] ? vec_num_x_[0] : vec_num_x_[1]; }
    int getMaxNumY() const noexcept { return num_y_; }

private:
    cv::Vec<cv::Point2d, 2> heads_;
    double interval_x_;
    double interval_y_;
    cv::Vec<int, 2> vec_num_x_;
    int num_y_;
};

class MicroImageRanges::iterator
{
public:
    iterator(const cv::Vec<cv::Point2d, 2>& heads, double interval_x, double interval_y, int index_x, int index_y,
             const cv::Vec<int, 2>& vec_num_x, int num_y) noexcept;

    MicroImage fromIndex(int index_x, int index_y) const noexcept;

    iterator& operator++() noexcept;
    bool operator!=(const iterator& rhs) const noexcept { return index_x_ != rhs.index_x_ || index_y_ != rhs.index_y_; }
    MicroImage operator*() const noexcept { return fromIndex(index_x_, index_y_); }

private:
    cv::Vec<cv::Point2d, 2> heads_;
    double interval_x_;
    double interval_y_;
    int index_x_;
    int index_y_;
    cv::Vec<int, 2> vec_num_x_;
    int num_y_;
};

} // namespace lvc