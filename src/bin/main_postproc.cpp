#include <filesystem>

#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <tlct.hpp>

#include "mca/impl/postproc.hpp"

namespace fs = std::filesystem;

template <typename TLayout>
    requires tlct::cfg::concepts::CLayout<TLayout>
void mainProc(const tlct::ConfigMap& cfg_map)
{
    using TParamConfig = tlct::_cfg::ParamConfig_<TLayout>;

    auto param_cfg = TParamConfig::fromConfigMap(cfg_map);

    const auto& generic_cfg = param_cfg.getGenericCfg();
    const auto layout = TLayout::fromCalibAndSpecConfig(param_cfg.getCalibCfg(), param_cfg.getSpecificCfg());

    const auto dstdir = fs::path{generic_cfg.getDstPattern()}.parent_path();
    fs::create_directories(dstdir);

    cv::Mat transposed_src;
    const cv::Range range = generic_cfg.getRange();
    for (int i = range.start; i <= range.end; i++) {
        const auto srcpath = generic_cfg.fmtSrcPath(i);

        const cv::Mat& src = cv::imread(srcpath.string());
        layout.processInto(src, transposed_src);

        const double crop_ratio = cfg_map.get<double, "crop_ratio">();
        const cv::Mat dst = mca::proc::postprocess(layout, transposed_src, crop_ratio);

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
        mainProc<tn::Layout>(cfg_map);
    } else {
        namespace tn = tlct::tspc;
        mainProc<tn::Layout>(cfg_map);
    }
}
