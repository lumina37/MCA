#include <array>
#include <cstdio>
#include <filesystem>
#include <numbers>

#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <tlct.hpp>

#include "mca/impl/postproc.hpp"

namespace fs = std::filesystem;

template <tlct::concepts::CLayout TLayout>
void mainProc(const tlct::ConfigMap& cfg_map)
{
    using TParamConfig = tlct::cfg::ParamConfig_<TLayout>;

    auto param_cfg = TParamConfig::fromConfigMap(cfg_map);

    const auto& generic_cfg = param_cfg.getGenericCfg();
    const auto layout = TLayout::fromCalibAndSpecConfig(param_cfg.getCalibCfg(), param_cfg.getSpecificCfg());

    const auto dstdir = generic_cfg.getDstPath().parent_path();
    fs::create_directories(dstdir);

    cv::Mat transposed_src;
    const char srcpath[256]{};
    const char dstpath[256]{};
    const cv::Range range = generic_cfg.getRange();
    for (int i = range.start; i <= range.end; i++) {
        sprintf((char*)srcpath, generic_cfg.getSrcPath().string().c_str(), i);

        const cv::Mat& src = cv::imread(srcpath);
        layout.processInto(src, transposed_src);

        const auto crop_ratio = cfg_map.get<"crop_ratio">(1. / std::numbers::sqrt2);
        const cv::Mat dst = mca::proc::postprocess(layout, transposed_src, crop_ratio);

        sprintf((char*)dstpath, generic_cfg.getDstPath().string().c_str(), i);
        cv::imwrite(dstpath, dst);
    }
}

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("MCA-PostProc", mca_VERSION, argparse::default_arguments::all);
    program.add_argument("param_file_path").help("the MCA parameter file path").required();
    program.add_epilog(MCA_COMPILE_INFO);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    const auto& param_file_path = program.get<std::string>("param_file_path");
    const auto cfg_map = tlct::ConfigMap::fromPath(param_file_path);

    constexpr std::array<void (*)(const tlct::ConfigMap&), tlct::PIPELINE_COUNT> handlers{
        mainProc<tlct::raytrix::Layout>,
        mainProc<tlct::tspc::Layout>,
    };
    const auto& handler = handlers[cfg_map.getPipelineType()];
    handler(cfg_map);
}
