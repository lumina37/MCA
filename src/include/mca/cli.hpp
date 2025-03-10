#pragma once

#include <filesystem>
#include <memory>
#include <numbers>
#include <string>
#include <utility>

#include <argparse/argparse.hpp>

#include "mca/common/defines.h"

namespace mca {

namespace _cfg {

namespace fs = std::filesystem;

[[nodiscard]] static inline std::unique_ptr<argparse::ArgumentParser> makeUniqArgParser() noexcept {
    auto parser = std::make_unique<argparse::ArgumentParser>("mca", "v" mca_VERSION, argparse::default_arguments::all);

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

    parser->add_epilog(MCA_COMPILE_INFO);

    return parser;
}

struct CliConfig {
    struct Path {
        fs::path src;
        fs::path dst;
    };

    struct Range {
        int begin;
        int end;
    };

    struct Proc {
        float cropRatio;
        int isPost;
    };

    Path path;
    Range range;
    Proc proc;

    // Initialize from
    [[nodiscard]] static inline CliConfig fromParser(const argparse::ArgumentParser& parser);
};

CliConfig CliConfig::fromParser(const argparse::ArgumentParser& parser) {
    auto path = CliConfig::Path{parser.get<std::string>("--src"), parser.get<std::string>("--dst")};
    auto range = CliConfig::Range{parser.get<int>("--begin"), parser.get<int>("--end")};
    auto proc = CliConfig::Proc{parser.get<float>("--cropRatio"), parser.get<bool>("--post")};

    return {std::move(path), std::move(range), std::move(proc)};
}

}  // namespace _cfg

namespace cfg {

namespace _ = _cfg;

using _::CliConfig;
using _::makeUniqArgParser;

}  // namespace cfg

}  // namespace mca
