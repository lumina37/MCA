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
    cv::Point num;
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
        center = cv::Point2d(center.y, center.x);
        for (int i = 0; i < 3; ++i) {
            offset[i] = cv::Point2d(offset[i].y, offset[i].x);
        }
    }

    // Calculation Number of Micro Lens
    int upNum = (int)((center.y - radius) / (radius * dSQRT3));
    int downNum = (int)((height - (center.y + radius)) / (radius * dSQRT3));
    num.y = upNum + 1 + downNum;
    double leftNum = (center.x - radius) / diameter;
    double rightNum = (width - (center.x + radius)) / diameter;

    if (((leftNum - (int)leftNum) > 0.5) || ((rightNum - (int)rightNum) >= 0.5))
        num.x = (int)leftNum + 1 + (int)rightNum;
    else
        num.x = (int)leftNum + (int)rightNum;
    num.x = (int)(num.x / 3);

    // Calculation subGridRefPos
    for (int type = 0; type < 3; type++) {
        double refPos =
            center.x + diameter * -(offset[type].x +
                                    offset[type].y); // On the assumption that either offset[].x or offset[].y is Zero.
        double dis = diameter * 3;
        subGridRefPos[upNum % 2][type].x = refPos - (int)((refPos - radius) / dis) * dis;
        refPos = center.y;
        dis = diameter * dSQRT3;
        subGridRefPos[upNum % 2][type].y = refPos - (int)((refPos - radius) / dis) * dis;

        refPos = center.x + diameter * -(offset[type].x + offset[type].y) - (radius + diameter);
        dis = diameter * 3;
        subGridRefPos[abs(upNum % 2 - 1)][type].x = refPos - (int)((refPos - radius) / dis) * dis;
        refPos = center.y - radius * dSQRT3;
        dis = diameter * dSQRT3;
        subGridRefPos[abs(upNum % 2 - 1)][type].y = refPos - (int)((refPos - radius) / dis) * dis;
    }

    cv::Point2d shift[2];
    shift[0].x = std::min(subGridRefPos[0][0].x, std::min(subGridRefPos[0][1].x, subGridRefPos[0][2].x));
    shift[0].y = std::min(subGridRefPos[0][0].y, std::min(subGridRefPos[0][1].y, subGridRefPos[0][2].y));
    shift[1].x = std::min(subGridRefPos[1][0].x, std::min(subGridRefPos[1][1].x, subGridRefPos[1][2].x));
    shift[1].y = std::min(subGridRefPos[1][0].y, std::min(subGridRefPos[1][1].y, subGridRefPos[1][2].y));
    double start_x = shift[0].x;
    double start_y = shift[0].y;
    bool is_right_shift = shift[0].x < shift[1].x;
    bool is_horizontal = rotation <= PI / 4.0;

    if (rotation > PI / 4.0) {
        double tmp = start_x;
        start_x = start_y;
        start_y = tmp;
    }

    return {diameter,       width,         height, start_x, start_y,
            is_right_shift, is_horizontal, 0.0,    0.0,     square_width_diam_ratio};
}

} // namespace lvc