#include <catch2/catch_test_macros.hpp>

#include "LVC/common/config_reader.h"

TEST_CASE("Read config from param.cfg", "[config]")
{
    auto cfg =
        fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230508_LVC\code\rlc-master\TestDataset\ChessPieces\param_pre.cfg)");

    REQUIRE(cfg.getWidth() == 3840);
    REQUIRE(cfg.getHeight() == 2160);
}
