#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include <opencv2/core.hpp>
#include <pugixml.hpp>

#include "mca/common/const.h"
#include "mca/config/config.h"
#include "mca/config/parser.h"

namespace mca {

Config fromRaytrixCfgFilePath(const std::string& cfg_file_path)
{
    /* Read param.cfg */

    std::ifstream file(cfg_file_path);
    std::map<std::string, std::string> cfg_map;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream row(line);
        std::string key;
        if (std::getline(row, key, '\t')) {
            std::string value;
            if (std::getline(row, value)) {
                cfg_map[key] = value;
            }
        }
    }

    std::string Calibration_xml = cfg_map["Calibration_xml"];
    int height = std::stoi(cfg_map["height"]);
    int width = std::stoi(cfg_map["width"]);
    double crop_ratio = std::stod(cfg_map["crop_ratio"]);

    file.close();

    /* Read XML */

    pugi::xml_document doc;
    doc.load_file(Calibration_xml.c_str());
    auto root_node = doc.child("RayCalibData");

    double diameter = root_node.child("diameter").text().as_double();
    double rotation = root_node.child("rotation").text().as_double();
    bool is_rotated = static_cast<bool>(rotation > PI / 4.0);

    double offset_x = root_node.child("offset").child("x").text().as_double();
    double offset_y = root_node.child("offset").child("y").text().as_double();
    cv::Point2d center{width / 2.0 + offset_x, height / 2.0 - offset_y};

    return {diameter, width, height, center, crop_ratio, is_rotated};
}

} // namespace mca