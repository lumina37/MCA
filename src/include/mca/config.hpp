#pragma once

#include <fstream>
#include <map>
#include <sstream>

#include <opencv2/core.hpp>
#include <pugixml.hpp>

#include "mca/common/defines.h"
#include "mca/helper.hpp"

namespace mca {

class MCA_API Config
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

    static Config fromRaytrixCfgFilePath(const std::string_view cfg_file_path);

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

inline Config::Config(double diameter, int width, int height, cv::Point2d point, double crop_ratio,
                      bool is_rotated) noexcept
    : diameter_(diameter), width_(width), height_(height), crop_ratio_(crop_ratio), is_rotated_(is_rotated)
{
    double radius = diameter / 2.0;
    // `line` is ALWAYS contiguous.
    // If `is_rotated==false`, just like *chess*, then `line` is horizontal.
    // Otherwise, aiming to keep `line` contiguous, it is vertical.
    double elem_interval = diameter;
    double line_interval = diameter * dSQRT3_2;

    if (!is_rotated) {
        int line_idx = static_cast<int>((point.y - radius) / line_interval);
        line_starts_[line_idx % 2].x = point.x - floor((point.x - radius) / elem_interval) * elem_interval;
        line_starts_[abs((line_idx % 2) - 1)].x = point.x - floor(point.x / elem_interval) * elem_interval + radius;
        line_starts_[0].y = point.y - line_idx * line_interval;
        line_starts_[1].y = line_starts_[0].y + line_interval;
    } else {
        int line_idx = static_cast<int>((point.x - radius) / line_interval);
        line_starts_[line_idx % 2].x = point.y - floor((point.y - radius) / elem_interval) * elem_interval;
        line_starts_[abs((line_idx % 2) - 1)].x = point.y - floor(point.y / elem_interval) * elem_interval + radius;
        line_starts_[0].y = point.x - line_idx * line_interval;
        line_starts_[1].y = line_starts_[0].y + line_interval;
    }
}

inline Config Config::fromRaytrixCfgFilePath(const std::string_view cfg_file_path)
{
    /* Read param.cfg */

    std::ifstream file(cfg_file_path.data());
    std::map<std::string, std::string> cfg_map;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream row(line);
        std::string key;
        if (std::getline(row, key, '\t')) {
            std::string value;
            if (std::getline(row, value)) {
                cfg_map[key] = value;
            }
        }
    }

    std::string Calibration_xml = cfg_map["Calibration_xml"];
    int height = std::stoi(cfg_map["height"]);
    int width = std::stoi(cfg_map["width"]);
    double crop_ratio = std::stod(cfg_map["square_width_diam_ratio"]);

    file.close();

    /* Read XML */

    pugi::xml_document doc;
    doc.load_file(Calibration_xml.c_str());
    auto root_node = doc.child("RayCalibData");

    double diameter = root_node.child("diameter").text().as_double();
    double rotation = root_node.child("rotation").text().as_double();
    bool is_rotated = static_cast<bool>(rotation > PI / 4.0);

    double offset_x = root_node.child("offset").child("x").text().as_double();
    double offset_y = root_node.child("offset").child("y").text().as_double();
    cv::Point2d center{width / 2.0 + offset_x, height / 2.0 - offset_y};

    return {diameter, width, height, center, crop_ratio, is_rotated};
}

class MCA_API MicroImage
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

inline MicroImage::MicroImage(double x, double y, int xidx, int yidx) noexcept
    : x_(x), y_(y), index_x_(xidx), index_y_(yidx)
{
}

class MCA_API MicroImageRanges
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

inline MicroImageRanges::MicroImageRanges(cv::Vec<cv::Point2d, 2> line_starts, cv::Vec2i elem_nums, int line_num,
                                          bool is_rotated) noexcept
    : line_starts_(line_starts), elem_nums_(elem_nums), line_num_(line_num), is_rotated_(is_rotated)
{
}

inline MicroImageRanges MicroImageRanges::fromConfig(const Config& cfg) noexcept
{
    auto line_starts = cfg.getLineStarts();
    double elem_interval = computeElemInterval(line_starts);
    double line_interval = computeLineInterval(line_starts);

    cv::Vec2i elem_nums;
    double elem_size_limit = cfg.getIsRotated() ? cfg.getHeight() : cfg.getWidth();
    double line_size_limit = cfg.getIsRotated() ? cfg.getWidth() : cfg.getHeight();

    for (int i = 0; i < 2; i++) {
        elem_nums[i] =
            static_cast<int>((elem_size_limit - line_starts[i].x - cfg.getDiameter() / 2) / elem_interval) + 1;
    }
    int line_num = static_cast<int>((line_size_limit - line_starts[0].y - cfg.getDiameter() / 2) / line_interval) + 1;

    return {line_starts, elem_nums, line_num, cfg.getIsRotated()};
}

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

MicroImageRanges::iterator::iterator(cv::Vec<cv::Point2d, 2> line_starts, double elem_interval, double line_interval,
                                     int elem_idx, int line_idx, cv::Vec2i elem_nums, int line_num,
                                     bool is_rotated) noexcept
    : line_starts_(line_starts), elem_interval_(elem_interval), line_interval_(line_interval), elem_idx_(elem_idx),
      line_idx_(line_idx), elem_nums_(elem_nums), line_num_(line_num), is_rotated_(is_rotated)
{
}

inline MicroImageRanges::iterator MicroImageRanges::begin() const noexcept
{
    return {
        line_starts_, computeElemInterval(line_starts_), computeLineInterval(line_starts_), 0, 0, elem_nums_, line_num_,
        is_rotated_};
}

inline MicroImageRanges::iterator MicroImageRanges::end() const noexcept
{
    return {line_starts_, 0.0, 0.0, 0, line_num_, elem_nums_, line_num_, is_rotated_};
}

inline MicroImageRanges::iterator& MicroImageRanges::iterator::operator++() noexcept
{
    elem_idx_ += 1;
    int elem_num = elem_nums_[line_idx_ % 2];
    if (elem_idx_ == elem_num) {
        elem_idx_ = 0;
        line_idx_ += 1;
    }
    return *this;
}

inline MicroImage MicroImageRanges::iterator::fromIndex(int elem_idx, int line_idx) const noexcept
{
    int line_idx_mod2 = line_idx % 2;
    double start_x = line_starts_[line_idx_mod2].x;
    double start_y = line_starts_[0].y;
    double x = start_x + static_cast<double>(elem_idx) * elem_interval_;
    double y = start_y + static_cast<double>(line_idx) * line_interval_;

    if (is_rotated_) {
        std::swap(x, y);
        std::swap(elem_idx, line_idx);
    }

    return {x, y, elem_idx, line_idx};
}

} // namespace mca