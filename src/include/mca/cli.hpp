#pragma once

#include <cassert>
#include <filesystem>

namespace mca {

namespace _cfg {

namespace fs = std::filesystem;

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

    inline CliConfig(Path path, Range range, Proc proc);
};

CliConfig::CliConfig(Path path, Range range, Proc proc) : path(path), range(range), proc(proc) {
    assert(range.end > range.begin);
    assert(proc.cropRatio > 0.0f);
}

}  // namespace _cfg

namespace cfg {

namespace _ = _cfg;

using _::CliConfig;

}  // namespace cfg

}  // namespace mca
