#include <array>
#include <filesystem>
#include <sstream>

#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <tlct.hpp>

#include "mca.hpp"

namespace fs = std::filesystem;

template <tlct::concepts::CLayout TLayout>
static inline void mainProc(const argparse::ArgumentParser& parser, const tlct::ConfigMap& map) {
    const auto& cli_cfg = mca::CliConfig::fromParser(parser);
    const auto layout = TLayout::fromCfgMap(map);

    cv::Size src_size, dst_size;
    decltype(mca::proc::preprocessInto<TLayout>)* proc_fn;

    if (cli_cfg.proc.is_post) {
        dst_size = layout.getRawImgSize();
        src_size = mca::proc::preprocOutputSize(layout, cli_cfg.proc.crop_ratio);
        if (layout.getDirection()) {
            std::swap(src_size.width, src_size.height);
        }
        proc_fn = mca::proc::postprocessInto<TLayout>;
    } else {
        src_size = layout.getRawImgSize();
        dst_size = mca::proc::preprocOutputSize(layout, cli_cfg.proc.crop_ratio);
        if (layout.getDirection()) {
            std::swap(dst_size.width, dst_size.height);
        }
        proc_fn = mca::proc::preprocessInto<TLayout>;
    }

    const auto& dstdir = cli_cfg.path.dst;
    fs::create_directories(dstdir);

    std::stringstream filename_s;
    filename_s << "mca-" << dst_size.width << 'x' << dst_size.height << ".yuv";
    fs::path saveto_path = dstdir / filename_s.str();
    auto yuv_writer = tlct::io::Yuv420Writer::fromPath(saveto_path);
    auto yuv_reader = tlct::io::Yuv420Reader::fromPath(cli_cfg.path.src, src_size.width, src_size.height);
    yuv_reader.skip(cli_cfg.range.begin);

    auto src_frame = tlct::io::Yuv420Frame{src_size};
    auto dst_frame = tlct::io::Yuv420Frame{dst_size};

    cv::Mat upsampled_uv, transposed_y, transposed_uv, processed;
    for (int i = cli_cfg.range.begin; i < cli_cfg.range.end; i++) {
        yuv_reader.read_into(src_frame);

        constexpr int UV_UPSAMPLE = 2;

        if (layout.getDirection()) {
            cv::transpose(src_frame.getY(), transposed_y);
            proc_fn(layout, transposed_y, processed, cli_cfg.proc.crop_ratio);
            cv::transpose(processed, dst_frame.getY());

            cv::transpose(src_frame.getU(), transposed_uv);
            cv::resize(transposed_uv, upsampled_uv, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            proc_fn(layout, upsampled_uv, processed, cli_cfg.proc.crop_ratio);
            cv::resize(processed, transposed_uv, {(int)dst_frame.getUHeight(), (int)dst_frame.getUWidth()}, 0.0, 0.0,
                       cv::INTER_AREA);
            cv::transpose(transposed_uv, dst_frame.getU());

            cv::transpose(src_frame.getV(), transposed_uv);
            cv::resize(transposed_uv, upsampled_uv, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            proc_fn(layout, upsampled_uv, processed, cli_cfg.proc.crop_ratio);
            cv::resize(processed, transposed_uv, {(int)dst_frame.getVHeight(), (int)dst_frame.getVWidth()}, 0.0, 0.0,
                       cv::INTER_AREA);
            cv::transpose(transposed_uv, dst_frame.getV());
        } else {
            transposed_y = src_frame.getY();

            proc_fn(layout, src_frame.getY(), dst_frame.getY(), cli_cfg.proc.crop_ratio);

            cv::resize(src_frame.getU(), upsampled_uv, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            proc_fn(layout, upsampled_uv, processed, cli_cfg.proc.crop_ratio);
            cv::resize(processed, dst_frame.getU(), {(int)dst_frame.getUWidth(), (int)dst_frame.getUHeight()}, 0.0, 0.0,
                       cv::INTER_AREA);

            cv::resize(src_frame.getV(), upsampled_uv, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            proc_fn(layout, upsampled_uv, processed, cli_cfg.proc.crop_ratio);
            cv::resize(processed, dst_frame.getV(), {(int)dst_frame.getVWidth(), (int)dst_frame.getVHeight()}, 0.0, 0.0,
                       cv::INTER_AREA);
        }

        yuv_writer.write(dst_frame);
    }
}

int main(int argc, char* argv[]) {
    auto parser = mca::makeParser();

    try {
        parser->parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << *parser;
        std::exit(1);
    }

    constexpr std::array<void (*)(const argparse::ArgumentParser&, const tlct::ConfigMap&), 2> handlers{
        mainProc<tlct::raytrix::Layout>,
        mainProc<tlct::tspc::Layout>,
    };

    try {
        const auto& calib_file_path = parser->get<std::string>("calib_file");
        const auto& map = tlct::ConfigMap::fromPath(calib_file_path);
        const int pipeline = ((map.get_or<"IsKepler">(0) << 1) | map.get_or<"IsMultiFocus">(0)) - 1;
        const auto& handler = handlers[pipeline];
        handler(*parser, map);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::exit(2);
    }
}
