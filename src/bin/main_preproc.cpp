#include <filesystem>

#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <tlct.hpp>

#include "mca/impl/preproc.hpp"

namespace fs = std::filesystem;
namespace tcpt = tlct::cfg::concepts;

template <typename TParamConfig, typename TLayout>
    requires tcpt::CSpecificConfig<typename TParamConfig::TSpecificConfig> &&
             tcpt::CCalibConfig<typename TParamConfig::TCalibConfig> && tcpt::CLayout<TLayout>
void mainProc(const tlct::ConfigMap& cfg_map)
{
    auto param_cfg = TParamConfig::fromConfigMap(cfg_map);

    const auto& generic_cfg = param_cfg.getGenericCfg();
    const auto layout = TLayout::fromParamConfig(param_cfg);

    const auto dstdir = fs::path{generic_cfg.getDstPattern()}.parent_path();
    fs::create_directories(dstdir);

    const cv::Range range = generic_cfg.getRange();
    for (int i = range.start; i <= range.end; i++) {
        const auto srcpath = generic_cfg.fmtSrcPath(i);

        const cv::Mat& src = cv::imread(srcpath.string());
        const cv::Mat transposed_src = layout.procImg(src);

        const double crop_ratio = std::stod(cfg_map.getMap().at("crop_ratio"));
        const cv::Mat dst = mca::proc::preprocess(layout, transposed_src, crop_ratio);

        const auto dstpath = generic_cfg.fmtDstPath(i);
        cv::imwrite(dstpath.string(), dst);
    }
}

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("MCA", MCA_GIT_TAG, argparse::default_arguments::all);
    program.add_argument("param_file_path").help("the MCA parameter file path").required();
    program.add_description(MCA_COMPILE_INFO);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    const auto& param_file_path = program.get<std::string>("param_file_path");
    const auto cfg_map = tlct::ConfigMap::fromPath(param_file_path);

    if (cfg_map.getPipelineType() == tlct::PipelineType::RLC) {
        namespace tn = tlct::raytrix;
        mainProc<tn::ParamConfig, tn::Layout>(cfg_map);
    } else {
        namespace tn = tlct::tspc;
        mainProc<tn::ParamConfig, tn::Layout>(cfg_map);
    }
}
