#include <gtest/gtest.h>

#include "mca/config/parser.h"

TEST(Config, common)
{
    const auto cfg = mca::fromRaytrixCfgFilePath(R"(D:\Code\SIGS\230818_VVC\dataset\videos\ChessPieces\param.cfg)");

    EXPECT_FLOAT_EQ(cfg.getDiameter(), 34.97973633);
    REQUIRE(cfg.getWidth() == 3840);
    REQUIRE(cfg.getHeight() == 2160);
    EXPECT_FLOAT_EQ(cfg.getLineStarts()[0].x, 27.25284766);
    EXPECT_FLOAT_EQ(cfg.getLineStarts()[0].y, 27.24191481);
}
