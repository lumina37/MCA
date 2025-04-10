#pragma once

#include <memory>
#include <numbers>
#include <string>

#include <argparse/argparse.hpp>

#include "mca.hpp"

[[nodiscard]] static inline std::unique_ptr<argparse::ArgumentParser> makeUniqArgParser() noexcept {
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
        .default_value(1 / std::numbers::sqrt2);

    parser->add_epilog(std::string{mca::compileInfo});

    return parser;
}

[[nodiscard]] static inline mca::CliConfig cfgFromCliParser(const argparse::ArgumentParser& parser) {
    return {mca::CliConfig::Path{parser.get<std::string>("--src"), parser.get<std::string>("--dst")},
            mca::CliConfig::Range{parser.get<int>("--begin"), parser.get<int>("--end")},
            mca::CliConfig::Proc{parser.get<float>("--cropRatio"), parser.get<bool>("--post")}};
}