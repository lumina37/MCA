#include <array>
#include <exception>
#include <expected>
#include <filesystem>
#include <iostream>
#include <numbers>
#include <sstream>
#include <string>
#include <utility>

#include <argparse/argparse.hpp>
#include <opencv2/core.hpp>
#include <tlct.hpp>

#include "mca.hpp"
#include "mca_bin_helper.hpp"

namespace fs = std::filesystem;

[[nodiscard]] static std::unique_ptr<argparse::ArgumentParser> makeUniqArgParser() noexcept {
    auto parser = std::make_unique<argparse::ArgumentParser>("mca", std::string("v").append(mca::version),
                                                             argparse::default_arguments::all);

    parser->set_usage_max_line_width(120);
    parser->add_argument("calib_file").help("path of the `calib.cfg`").required();
    parser->add_group("I/O");
    parser->add_argument("-i", "--src").help("input yuv420p file").required();
    parser->add_argument("-o", "--dst").help("output directory").required();
    parser->add_group("Frame Range");
    parser->add_argument("-b", "--begin")
        .help("the index of the start frame, left contains, starts from zero")
        .scan<'i', int>()
        .default_value(0);
    parser->add_argument("-e", "--end")
        .help("the index of the end frame, right NOT contains")
        .scan<'i', int>()
        .default_value(1);
    parser->add_group("Conversion");
    parser->add_argument("--post").help("post process instead of preprocess").flag().required();
    parser->add_argument("--cropRatio")
        .help("the ratio between cropping width and the diameter of Micro Image")
        .scan<'g', float>()
        .default_value(1.f / std::numbers::sqrt2_v<float>);

    parser->add_epilog(std::string{mca::compileInfo});

    return parser;
}

[[nodiscard]] static mca::CliConfig cfgFromCliParser(const argparse::ArgumentParser &parser) {
    return {mca::CliConfig::Path{parser.get<std::string>("--src"), parser.get<std::string>("--dst")},
            mca::CliConfig::Range{parser.get<int>("--begin"), parser.get<int>("--end")},
            mca::CliConfig::Proc{parser.get<float>("--cropRatio"), parser.get<bool>("--post")}};
}

template <tlct::concepts::CArrange TArrange>
static std::expected<void, tlct::Error> mainProc(const mca::CliConfig &cliCfg, const tlct::ConfigMap &map) {
    auto arrangeRes = TArrange::createWithCfgMap(map);
    if (!arrangeRes) return std::unexpected{std::move(arrangeRes.error())};
    auto &arrange = arrangeRes.value();

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

    auto srcExtentRes = tlct::io::YuvPlanarExtent::createYuv420p8bit(srcSize.width, srcSize.height);
    if (!srcExtentRes) return std::unexpected{std::move(srcExtentRes.error())};
    auto srcExtent = srcExtentRes.value();

    auto dstExtentRes = tlct::io::YuvPlanarExtent::createYuv420p8bit(dstSize.width, dstSize.height);
    if (!dstExtentRes) return std::unexpected{std::move(dstExtentRes.error())};
    auto dstExtent = dstExtentRes.value();

    auto yuvReaderRes = tlct::io::YuvPlanarReader::create(cliCfg.path.src, srcExtent);
    if (!yuvReaderRes) return std::unexpected{std::move(yuvReaderRes.error())};
    auto &yuvReader = yuvReaderRes.value();

    auto yuvWriterRes = tlct::io::YuvPlanarWriter::create(savetoPath);
    if (!yuvWriterRes) return std::unexpected{std::move(yuvWriterRes.error())};
    auto &yuvWriter = yuvWriterRes.value();

    auto skipRes = yuvReader.skip(cliCfg.range.begin);
    if (!skipRes) return std::unexpected{std::move(skipRes.error())};

    auto srcFrameRes = tlct::io::YuvPlanarFrame::create(srcExtent);
    if (!srcFrameRes) return std::unexpected{std::move(srcFrameRes.error())};
    auto &srcFrame = srcFrameRes.value();

    auto dstFrameRes = tlct::io::YuvPlanarFrame::create(dstExtent);
    if (!dstFrameRes) return std::unexpected{std::move(dstFrameRes.error())};
    auto &dstFrame = dstFrameRes.value();

    cv::Mat upsampledUV, transposedY, transposedUV, processed;
    for (int i = cliCfg.range.begin; i < cliCfg.range.end; i++) {
        auto readRes = yuvReader.readInto(srcFrame);
        if (!readRes) return std::unexpected{std::move(readRes.error())};

        constexpr int UV_UPSAMPLE = 2;

        if (arrange.getDirection()) {
            cv::transpose(srcFrame.getY(), transposedY);
            procFn(arrange, transposedY, processed, cliCfg.proc.cropRatio);
            cv::transpose(processed, dstFrame.getY());

            cv::transpose(srcFrame.getU(), transposedUV);
            cv::resize(transposedUV, upsampledUV, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            procFn(arrange, upsampledUV, processed, cliCfg.proc.cropRatio);
            cv::resize(processed, transposedUV, {dstExtent.getUHeight(), dstExtent.getUWidth()}, 0.0, 0.0,
                       cv::INTER_AREA);
            cv::transpose(transposedUV, dstFrame.getU());

            cv::transpose(srcFrame.getV(), transposedUV);
            cv::resize(transposedUV, upsampledUV, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            procFn(arrange, upsampledUV, processed, cliCfg.proc.cropRatio);
            cv::resize(processed, transposedUV, {dstExtent.getVHeight(), dstExtent.getVWidth()}, 0.0, 0.0,
                       cv::INTER_AREA);
            cv::transpose(transposedUV, dstFrame.getV());
        } else {
            transposedY = srcFrame.getY();

            procFn(arrange, srcFrame.getY(), dstFrame.getY(), cliCfg.proc.cropRatio);

            cv::resize(srcFrame.getU(), upsampledUV, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            procFn(arrange, upsampledUV, processed, cliCfg.proc.cropRatio);
            cv::resize(processed, dstFrame.getU(), {dstExtent.getUWidth(), dstExtent.getUHeight()}, 0.0, 0.0,
                       cv::INTER_AREA);

            cv::resize(srcFrame.getV(), upsampledUV, {}, UV_UPSAMPLE, UV_UPSAMPLE, cv::INTER_CUBIC);
            procFn(arrange, upsampledUV, processed, cliCfg.proc.cropRatio);
            cv::resize(processed, dstFrame.getV(), {dstExtent.getVWidth(), dstExtent.getVHeight()}, 0.0, 0.0,
                       cv::INTER_AREA);
        }

        auto writeRes = yuvWriter.write(dstFrame);
        if (!writeRes) return std::unexpected{std::move(writeRes.error())};
    }

    return {};
}

int main(int argc, char *argv[]) {
    auto parser = makeUniqArgParser();

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
        const auto &cliCfg = cfgFromCliParser(*parser);
        const auto &map = tlct::ConfigMap::createFromPath(calibFilePath) | unwrap;
        const int pipeline = ((map.getOr<"IsKepler">(0) << 1) | map.getOr<"IsMultiFocus">(0)) - 1;
        const auto &handler = handlers[pipeline];
        handler(cliCfg, map) | unwrap;
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::exit(2);
    }
}
