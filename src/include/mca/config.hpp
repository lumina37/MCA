#pragma once

#include <cstdio>
#include <filesystem>
#include <string>

#include <opencv2/core.hpp>

#include "tlct/common/defines.h"
#include "tlct/config.hpp"
#include "tlct/config/common.hpp"

namespace mca::cfg {

namespace fs = std::filesystem;
namespace tcfg = tlct::cfg;

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
class ParamConfig
{
public:
    TLCT_API ParamConfig() noexcept : calib_cfg_(), crop_ratio_(), imgsize_(), range_(), src_pattern_(), dst_dir_() {};
    TLCT_API ParamConfig& operator=(const ParamConfig& cfg) noexcept = default;
    TLCT_API ParamConfig(const ParamConfig& cfg) noexcept = default;
    TLCT_API ParamConfig& operator=(ParamConfig&& cfg) noexcept = default;
    TLCT_API ParamConfig(ParamConfig&& cfg) noexcept = default;
    TLCT_API ParamConfig(TCalibConfig&& calib_cfg, double crop_ratio, cv::Size imgsize, cv::Range range,
                         std::string src_pattern, const std::string& dst_dir) noexcept
        : calib_cfg_(calib_cfg), crop_ratio_(crop_ratio), imgsize_(imgsize), range_(range),
          src_pattern_(std::move(src_pattern)), dst_dir_(dst_dir) {};

    [[nodiscard]] TLCT_API static ParamConfig fromCommonCfg(const tcfg::CommonParamConfig& cfg);

    [[nodiscard]] TLCT_API const TCalibConfig& getCalibCfg() const noexcept;
    [[nodiscard]] TLCT_API double getCropRatio() const noexcept;
    [[nodiscard]] TLCT_API cv::Size getImgSize() const noexcept;
    [[nodiscard]] TLCT_API cv::Range getRange() const noexcept;
    [[nodiscard]] TLCT_API const std::string& getSrcPattern() const noexcept;
    [[nodiscard]] TLCT_API const fs::path& getDstDir() const noexcept;

private:
    TCalibConfig calib_cfg_;
    double crop_ratio_;
    cv::Size imgsize_;
    cv::Range range_;
    std::string src_pattern_;
    fs::path dst_dir_;
};

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
inline ParamConfig<TCalibConfig> ParamConfig<TCalibConfig>::fromCommonCfg(const tcfg::CommonParamConfig& cfg)
{
    const auto& cfg_map = cfg.getConfigMap();
    auto calib_cfg = TCalibConfig::fromXMLPath(cfg_map.at("Calibration_xml").c_str());
    const double crop_ratio = std::stod(cfg_map.at("crop_ratio"));
    const int width = std::stoi(cfg_map.at("width"));
    const int height = std::stoi(cfg_map.at("height"));
    const int start = std::stoi(cfg_map.at("start_frame"));
    const int end = std::stoi(cfg_map.at("end_frame"));
    const std::string& src_pattern = cfg_map.at("RawImage_Path");
    const std::string& dst_dir = cfg_map.at("Output_Path");
    return {std::move(calib_cfg), crop_ratio, {width, height}, {start, end}, src_pattern, dst_dir};
}

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
inline const TCalibConfig& ParamConfig<TCalibConfig>::getCalibCfg() const noexcept
{
    return calib_cfg_;
}

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
inline double ParamConfig<TCalibConfig>::getCropRatio() const noexcept
{
    return crop_ratio_;
}

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
inline cv::Size ParamConfig<TCalibConfig>::getImgSize() const noexcept
{
    return imgsize_;
}

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
inline cv::Range ParamConfig<TCalibConfig>::getRange() const noexcept
{
    return range_;
}

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
inline const std::string& ParamConfig<TCalibConfig>::getSrcPattern() const noexcept
{
    return src_pattern_;
}

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
inline const fs::path& ParamConfig<TCalibConfig>::getDstDir() const noexcept
{
    return {dst_dir_};
}

template <typename TCalibConfig>
    requires tcfg::concepts::CCalibConfig<TCalibConfig>
static inline fs::path fmtSrcPath(const ParamConfig<TCalibConfig>& cfg, int i) noexcept
{
    char buffer[256];
    sprintf(buffer, cfg.getSrcPattern().c_str(), i);
    return {buffer};
}

template class ParamConfig<tcfg::tspc::CalibConfig>;
template class ParamConfig<tcfg::raytrix::CalibConfig>;

} // namespace mca::cfg
