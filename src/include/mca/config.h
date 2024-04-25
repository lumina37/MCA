#pragma once

#include <opencv2/core.hpp>

#include "mca/common/defines.h"
#include "mca/process/helper.hpp"

namespace mca {

class LVC_EXPORT Config
{
public:
    /**
     * @brief 存储一切配置参数
     *
     * @param diameter microimage的直径
     * @param width 原始图像的宽度
     * @param height 原始图像的高度
     * @param point 任意一个MI的坐标
     * @param crop_ratio 裁切正方形宽度与直径的比值
     * @param is_rotated 相机是否经过旋转
     */
    Config(double diameter, int width, int height, cv::Point2d point, double crop_ratio = d1_SQRT2,
           bool is_rotated = false) noexcept;

    static Config fromRaytrixCfgFilePath(const std::string& cfg_file_path);

    double getDiameter() const noexcept { return diameter_; };
    int getWidth() const noexcept { return width_; };
    int getHeight() const noexcept { return height_; };
    cv::Vec<cv::Point2d, 2> getLineStarts() const noexcept { return line_starts_; };
    double getCropRatio() const noexcept { return crop_ratio_; };
    bool getIsRotated() const noexcept { return is_rotated_; }

private:
    double diameter_;
    int width_;
    int height_;
    cv::Vec<cv::Point2d, 2> line_starts_;
    double crop_ratio_;
    bool is_rotated_;
};

class LVC_EXPORT MicroImage
{
public:
    /**
     * @brief 用于对MI定位
     *
     * @param x MI的x坐标
     * @param y MI的y坐标
     * @param xidx MI的x轴索引值
     * @param yidx MI的y轴索引值
     *
     * @note x对应width对应columns y对应height对应rows
     */
    MicroImage(double x, double y, int xidx, int yidx) noexcept;

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
     * @param line_starts 头两条line的起始坐标
     * @param elem_nums 头两条line包含的elem数量
     * @param line_num line数量
     * @param is_rotated 相机是否经过旋转
     */
    MicroImageRanges(cv::Vec<cv::Point2d, 2> line_starts, cv::Vec2i elem_nums, int line_num, bool is_rotated) noexcept;

    class iterator;

    /**
     * @brief 从已有的`Config`生成用于遍历MI的`MicroImageRanges`
     * @param cfg 感觉不用解释
     * @return MicroImageRanges
     */
    static MicroImageRanges fromConfig(const Config& cfg) noexcept;

    iterator begin() const noexcept;
    iterator end() const noexcept;

    int getElemNumi(int row) const noexcept { return elem_nums_[row]; }
    int getMaxElemNum() const noexcept { return elem_nums_[0] > elem_nums_[1] ? elem_nums_[0] : elem_nums_[1]; }
    int getMaxLineNum() const noexcept { return line_num_; }

private:
    cv::Vec<cv::Point2d, 2> line_starts_;
    cv::Vec2i elem_nums_;
    int line_num_;
    bool is_rotated_;
};

class MicroImageRanges::iterator
{
public:
    iterator(cv::Vec<cv::Point2d, 2> line_starts, double elem_interval, double line_interval, int elem_idx,
             int line_idx, cv::Vec2i elem_nums, int line_num, bool is_rotated) noexcept;

    MicroImage fromIndex(int elem_idx, int line_idx) const noexcept;

    iterator& operator++() noexcept;
    bool operator!=(const iterator& rhs) const noexcept
    {
        return elem_idx_ != rhs.elem_idx_ || line_idx_ != rhs.line_idx_;
    }
    MicroImage operator*() const noexcept { return fromIndex(elem_idx_, line_idx_); }

private:
    cv::Vec<cv::Point2d, 2> line_starts_;
    double elem_interval_;
    double line_interval_;
    int elem_idx_;
    int line_idx_;
    cv::Vec2i elem_nums_;
    int line_num_;
    bool is_rotated_;
};

} // namespace mca