#include <catch2/catch_test_macros.hpp>

#include "LVC/config/parser.h"

TEST_CASE("Read config from param.cfg", "[config]")
{
    auto cfg = lvc::fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230818_VVC\dataset\videos\ChessPieces\param.cfg)");

    REQUIRE(cfg.getWidth() == 3840);
    REQUIRE(cfg.getHeight() == 2160);
}
