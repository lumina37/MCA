#include <filesystem>
#include <sstream>

#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <tlct/config.hpp>

#include "mca/common/cmake.h"
#include "mca/config.hpp"
#include "mca/impl/postproc.hpp"

namespace tcfg = tlct::cfg;
namespace fs = std::filesystem;

template <typename TLayout>
    requires tcfg::concepts::CLayout<TLayout>
void mainProc(const tcfg::CommonParamConfig& common_cfg)
{
    const auto param_cfg = mca::cfg::ParamConfig<typename TLayout::TCalibConfig>::fromCommonCfg(common_cfg);
    const auto& calib_cfg = param_cfg.getCalibCfg();
    const auto layout = TLayout::fromCfgAndImgsize(calib_cfg, param_cfg.getImgSize());

    fs::create_directories(param_cfg.getDstDir());

    const cv::Range range = param_cfg.getRange();
    for (int i = range.start; i <= range.end; i++) {
        const auto srcpath = mca::cfg::fmtSrcPath(param_cfg, i);

        const cv::Mat& src = cv::imread(srcpath.string());
        const cv::Mat transposed_src = TLayout::procImg(layout, src);

        const cv::Mat dst = mca::postprocess(layout, transposed_src);

        std::stringstream ss;
        ss << "frame#" << std::setw(3) << std::setfill('0') << i << ".png";
        const auto dstpath = param_cfg.getDstDir() / ss.str();
        cv::imwrite(dstpath.string(), dst);
    }
}

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("MCA", MCA_VERSION, argparse::default_arguments::all);
    program.add_argument("param_file_path").help("the MCA parameter file path").required();

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    const auto& param_file_path = program.get<std::string>("param_file_path");
    const auto common_cfg = tcfg::CommonParamConfig::fromPath(param_file_path.c_str());

    if (common_cfg.isTSPC()) {
        mainProc<tcfg::tspc::Layout>(common_cfg);
    } else {
        mainProc<tcfg::raytrix::Layout>(common_cfg);
    }
}
