#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <opencv2/core.hpp>
#include <pugixml.hpp>

#include "LVC/common/config.hpp"
#include "LVC/common/config_parser.h"
#include "LVC/common/const.h"

lvc::Config fromRaytrixCfgFilePath(const std::string& cfg_file_path)
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

    int viewNum = std::stoi(cfg_map["viewNum"]);
    int skipped_number_of_pixel_around_ML = std::stoi(cfg_map["rmode"]);
    int pmode = std::stoi(cfg_map["pmode"]);
    int mmode = std::stoi(cfg_map["mmode"]);
    int lmode = std::stoi(cfg_map["lmode"]);
    std::string Calibration_xml = cfg_map["Calibration_xml"];
    std::string RawImage_Path = cfg_map["RawImage_Path"];
    std::string Output_Path = cfg_map["Output_Path"];
    int debayer_model = std::stoi(cfg_map["Debayer_mode"]);
    int isfiltering = std::stoi(cfg_map["Isfiltering"]);
    int isCLAHE = std::stoi(cfg_map["isCLAHE"]);
    double gamma = std::stod(cfg_map["Gamma"]);
    double lambda = std::stod(cfg_map["Lambda"]);
    double sigma = std::stod(cfg_map["Sigma"]);
    int input_model = std::stoi(cfg_map["input_model"]);
    int output_model = std::stoi(cfg_map["output_model"]);
    int start_frame = std::stoi(cfg_map["start_frame"]);
    int end_frame = std::stoi(cfg_map["end_frame"]);
    int height = std::stoi(cfg_map["height"]);
    int width = std::stoi(cfg_map["width"]);
    double square_width_diam_ratio = std::stod(cfg_map["square_width_diam_ratio"]);

    file.close();

    /* Read XML */

    pugi::xml_document doc;
    doc.load_file(Calibration_xml.c_str());

    double diameter = doc.child("RayCalibData").child("diameter").text().as_double();
    double rotation = doc.child("RayCalibData").child("rotation").text().as_double();
    double radius = diameter * 0.5;
    cv::Point2d offset[3];
    cv::Point num;
    cv::Point2d center;
    cv::Point2d subGridRefPos[2][3];

    center.x = width * 0.5 + doc.child("RayCalibData").child("offset").child("x").text().as_double();
    center.y = height * 0.5 - doc.child("RayCalibData").child("offset").child("y").text().as_double();

    for (const auto& lenstype : doc.child("RayCalibData").children("lens_type")) {
        int id = lenstype.attribute("id").as_int();
        offset[id].x = lenstype.child("offset").child("x").text().as_double();
        offset[id].y = lenstype.child("offset").child("y").text().as_double();
    }

    /* Post Processing */

    // transpose each matrix
    if (rotation > lvc::PI / 4.0) {
        center = cv::Point2d(center.y, center.x);
        for (int i = 0; i < 3; ++i) {
            offset[i] = cv::Point2d(offset[i].y, offset[i].x);
        }
    }

    // Calculation Number of Micro Lens
    int upNum = (int)((center.y - radius) / (radius * sqrt(3)));
    int downNum = (int)((height - (center.y + radius)) / (radius * sqrt(3)));
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
        dis = diameter * sqrt(3);
        subGridRefPos[upNum % 2][type].y = refPos - (int)((refPos - radius) / dis) * dis;

        refPos = center.x + diameter * -(offset[type].x + offset[type].y) - (radius + diameter);
        dis = diameter * 3;
        subGridRefPos[abs(upNum % 2 - 1)][type].x = refPos - (int)((refPos - radius) / dis) * dis;
        refPos = center.y - radius * sqrt(3);
        dis = diameter * sqrt(3);
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
    bool is_horizontal = rotation <= lvc::PI / 4.0;

    if (rotation > lvc::PI / 4.0) {
        center = cv::Point2d(center.y, center.x);
        double tmp = start_x;
        start_x = start_y;
        start_y = tmp;
    }

    return {diameter,       width,         height, start_x, start_y,
            is_right_shift, is_horizontal, 0.0,    0.0,     square_width_diam_ratio};
}
