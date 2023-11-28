#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "MCA/config/parser.h"
#include "MCA/process/preproc.h"

int main()
{
    auto cfg = mca::fromRaytrixCfgFilePath(R"(D:\code\SIGS\230818_VVC\dataset\config\ChessPieces\param.cfg)");
    auto src = cv::imread("src_chess.png");
    cv::Mat dst;
    mca::preprocess(cfg, src, dst);
    cv::imwrite("forward.png", dst);
    return 0;
}
