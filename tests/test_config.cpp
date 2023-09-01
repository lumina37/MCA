#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "LVC/config/parser.h"

TEST_CASE("Read config from param.cfg", "[config]")
{
    auto cfg = lvc::fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230818_VVC\dataset\videos\ChessPieces\param.cfg)");

    REQUIRE_THAT(cfg.getDiameter(), Catch::Matchers::WithinAbs(34.97973633, 0.001));
    REQUIRE(cfg.getWidth() == 3840);
    REQUIRE(cfg.getHeight() == 2160);
    REQUIRE_THAT(cfg.getStartX(), Catch::Matchers::WithinAbs(27.25284766, 0.001));
    REQUIRE_THAT(cfg.getStartY(), Catch::Matchers::WithinAbs(27.24191481, 0.001));
    REQUIRE_THAT(cfg.getIntervalX(), Catch::Matchers::WithinAbs(34.97973633, 0.001));
    REQUIRE_THAT(cfg.getIntervalY(), Catch::Matchers::WithinAbs(30.29334028, 0.001));
    REQUIRE(cfg.getIsHorizontal() == true);
    REQUIRE(cfg.getIsRightShift() == true);
}
