#include <array>
#include <exception>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <tlct.hpp>

#include "mca.hpp"

namespace fs = std::filesystem;

template <tlct::concepts::CArrange TArrange>
static inline void mainProc(const argparse::ArgumentParser &parser, const tlct::ConfigMap &map) {
    const auto &cliCfg = mca::CliConfig::fromParser(parser);
    const auto arrange = TArrange::fromCfgMap(map);

    cv::Size srcSize, dstSize;
    decltype(mca::proc::preprocessInto<TArrange>) *procFn;

    if (cliCfg.proc.isPost) {
        dstSize = arrange.getImgSize();
        srcSize = mca::proc::preprocOutputSize(arrange, cliCfg.proc.cropRatio);
        if (arrange.getDirection()) {
            std::swap(srcSize.width, srcSize.height);
            std::swap(dstSize.width, dstSize.height);
        }
        procFn = mca::proc::postprocessInto<TArrange>;
    } else {
        srcSize = arrange.getImgSize();
        dstSize = mca::proc::preprocOutputSize(arrange, cliCfg.proc.cropRatio);
        if (arrange.getDirection()) {
            std::swap(srcSize.width, srcSize.height);
            std::swap(dstSize.width, dstSize.height);
        }
        procFn = mca::proc::preprocessInto<TArrange>;
    }

    const auto &dstdir = cliCfg.path.dst;
    fs::create_directories(dstdir);

    std::stringstream sFilename;
    sFilename << "mca-" << dstSize.width << 'x' << dstSize.height << ".yuv";
    fs::path savetoPath = dstdir / sFilename.str();
    auto yuvWriter = tlct::io::Yuv420Writer::fromPath(savetoPath);
    auto yuvReader = tlct::io::Yuv420Reader::fromPath(cliCfg.path.src, srcSize.width, srcSize.height);
    yuvReader.skip(cliCfg.range.begin);

    auto srcFrame = tlct::io::Yuv420Frame{srcSize};
    auto dstFrame = tlct::io::Yuv420Frame{dstSize};

    cv::Mat upsampledUV, transposedY, transposedUV, processed;
    for (int i = cliCfg.range.begin; i < cliCfg.range.end; i++) {
        yuvReader.readInto(srcFrame);

        constexpr int UV_UPSAMPLE = 2;

        if (arrange.getDirection()) {
            cv::transpose(srcFrame.getY(), transposedY);
            procFn(arrange, transposedY, processed, cliCfg.proc.cropRatio);
            cv::transpose(processed, dstFrame.getY());

            cv::transpose(srcFrame.getU(), transposedUV);
            cv::resize(transposedUV, upsampledUV, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            procFn(arrange, upsampledUV, processed, cliCfg.proc.cropRatio);
            cv::resize(processed, transposedUV, {(int)dstFrame.getUHeight(), (int)dstFrame.getUWidth()}, 0.0, 0.0,
                       cv::INTER_AREA);
            cv::transpose(transposedUV, dstFrame.getU());

            cv::transpose(srcFrame.getV(), transposedUV);
            cv::resize(transposedUV, upsampledUV, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            procFn(arrange, upsampledUV, processed, cliCfg.proc.cropRatio);
            cv::resize(processed, transposedUV, {(int)dstFrame.getVHeight(), (int)dstFrame.getVWidth()}, 0.0, 0.0,
                       cv::INTER_AREA);
            cv::transpose(transposedUV, dstFrame.getV());
        } else {
            transposedY = srcFrame.getY();

            procFn(arrange, srcFrame.getY(), dstFrame.getY(), cliCfg.proc.cropRatio);

            cv::resize(srcFrame.getU(), upsampledUV, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            procFn(arrange, upsampledUV, processed, cliCfg.proc.cropRatio);
            cv::resize(processed, dstFrame.getU(), {(int)dstFrame.getUWidth(), (int)dstFrame.getUHeight()}, 0.0, 0.0,
                       cv::INTER_AREA);

            cv::resize(srcFrame.getV(), upsampledUV, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            procFn(arrange, upsampledUV, processed, cliCfg.proc.cropRatio);
            cv::resize(processed, dstFrame.getV(), {(int)dstFrame.getVWidth(), (int)dstFrame.getVHeight()}, 0.0, 0.0,
                       cv::INTER_AREA);
        }

        yuvWriter.write(dstFrame);
    }
}

int main(int argc, char *argv[]) {
    auto parser = mca::makeUniqArgParser();

    try {
        parser->parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << *parser;
        std::exit(1);
    }

    constexpr std::array handlers{
        mainProc<tlct::raytrix::Arrange>,
        mainProc<tlct::tspc::Arrange>,
    };

    try {
        const auto &calibFilePath = parser->get<std::string>("calib_file");
        const auto &map = tlct::ConfigMap::fromPath(calibFilePath);
        const int pipeline = ((map.getOr<"IsKepler">(0) << 1) | map.getOr<"IsMultiFocus">(0)) - 1;
        const auto &handler = handlers[pipeline];
        handler(*parser, map);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::exit(2);
    }
}
