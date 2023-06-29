#include <iostream>

#include "__rlc_config.hpp"

#include "LVC/common/config.hpp"

int main()
{
    auto cfg = fromRlcCfgFilePath("D:\\Code\\SIGS\\230508_LVC\\code\\rlc-master\\TestDataset\\NagoyaFujita\\param.cfg");
    std::cout << cfg.getDiameter() << std::endl;
    return 0;
}
