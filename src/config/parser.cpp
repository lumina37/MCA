#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include <opencv2/core.hpp>
#include <pugixml.hpp>

#include "LVC/common/const.h"
#include "LVC/config/config.h"
#include "LVC/config/parser.h"

namespace lvc {

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
    std::string RawImage_Path = cfg_map["RawImage_Path"];
    std::string Output_Path = cfg_map["Output_Path"];
    int height = std::stoi(cfg_map["height"]);
    int width = std::stoi(cfg_map["width"]);
    double square_width_diam_ratio = std::stod(cfg_map["square_width_diam_ratio"]);

    file.close();

    /* Read XML */

    pugi::xml_document doc;
    doc.load_file(Calibration_xml.c_str());
    auto root_node = doc.child("RayCalibData");

    double diameter = root_node.child("diameter").text().as_double();
    double rotation = root_node.child("rotation").text().as_double();
    double radius = diameter * 0.5;
    cv::Point2d offset[3];
    cv::Point2d center;
    cv::Point2d subGridRefPos[2][3];

    center.x = width * 0.5 + root_node.child("offset").child("x").text().as_double();
    center.y = height * 0.5 - root_node.child("offset").child("y").text().as_double();

    for (const auto& lenstype : root_node.children("lens_type")) {
        int id = lenstype.attribute("id").as_int();
        offset[id].x = lenstype.child("offset").child("x").text().as_double();
        offset[id].y = lenstype.child("offset").child("y").text().as_double();
    }

    /* Post Processing */

    // transpose each matrix
    if (rotation > PI / 4.0) {
        std::swap(center.x, center.y);
        for (int i = 0; i < 3; ++i) {
            std::swap(offset[i].y, offset[i].x);
        }
    }

    // Calculation Number of Micro Lens
    int upNum = static_cast<int>((center.y - radius) / (radius * dSQRT3));

    // Calculation subGridRefPos
    constexpr double double_max = std::numeric_limits<double>::max();
    cv::Vec<cv::Point2d, 2> heads{{double_max, double_max}, {double_max, double_max}};
    auto update_head = [&heads](int i, double x, double y) {
        if (x < heads[i].x) {
            heads[i].x = x;
        }
        if (y < heads[i].y) {
            heads[i].y = y;
        }
    };

    for (int type = 0; type < 3; type++) {
        double ref_pos_x, ref_pos_y;
        double x, y;
        double dis_x = diameter * 3;
        double dis_y = diameter * dSQRT3;

        ref_pos_x = center.x - diameter * (offset[type].x + offset[type].y);
        x = ref_pos_x - (int)((ref_pos_x - radius) / dis_x) * dis_x;
        ref_pos_y = center.y;
        y = ref_pos_y - (int)((ref_pos_y - radius) / dis_y) * dis_y;
        update_head(upNum % 2, x, y);

        ref_pos_x = center.x + diameter * -(offset[type].x + offset[type].y) - (radius + diameter);
        x = ref_pos_x - (int)((ref_pos_x - radius) / dis_x) * dis_x;
        ref_pos_y = center.y - radius * dSQRT3;
        y = ref_pos_y - (int)((ref_pos_y - radius) / dis_y) * dis_y;
        update_head(abs(upNum % 2 - 1), x, y);
    }

    if (rotation > PI / 4.0) {
        std::swap(heads[0].x, heads[0].y);
        std::swap(heads[1].x, heads[1].y);
    }

    return {diameter, width, height, heads, square_width_diam_ratio};
}

} // namespace lvc