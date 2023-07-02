#pragma once

#include <cstdio>
#include <iostream>
#include <string>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "LVC/common/config.hpp"
#include "LVC/common/const.hpp"
#include "LVC/preprocess/backward.hpp"
#include "LVC/preprocess/forward.hpp"

namespace bst_ptree = boost::property_tree;

static lvc::Config fromRlcCfgFilePath(const std::string& cfg_file_path)
{
    char Calibration_xml[128];
    char RawImage_Path[128];
    char Output_Path[128];
    int viewNum, skipped_number_of_pixel_around_ML, pmode, mmode, lmode, debayer_model, isfiltering, isCLAHE,
        input_model, output_model, start_frame, end_frame, height, width;
    double gamma, lambda, sigma, square_width_diam_ratio;

    char s[256];
    FILE* fp = fopen(cfg_file_path.c_str(), "r");
    if (!fp) {
        std::cerr << "cannot open config file" << std::endl;
        exit(-1);
    }

    fscanf(fp, "%s\t%d", s, &viewNum);
    fscanf(fp, "%s\t%d", s, &skipped_number_of_pixel_around_ML);
    fscanf(fp, "%s\t%d", s, &pmode);
    fscanf(fp, "%s\t%d", s, &mmode);
    fscanf(fp, "%s\t%d", s, &lmode);
    fscanf(fp, "%s\t%s", s, Calibration_xml);
    fscanf(fp, "%s\t%s", s, RawImage_Path);
    fscanf(fp, "%s\t%s", s, Output_Path);
    fscanf(fp, "%s\t%d", s, &debayer_model);
    fscanf(fp, "%s\t%d", s, &isfiltering);
    fscanf(fp, "%s\t%d", s, &isCLAHE);
    fscanf(fp, "%s\t%lf", s, &gamma);
    fscanf(fp, "%s\t%lf", s, &lambda);
    fscanf(fp, "%s\t%lf", s, &sigma);
    fscanf(fp, "%s\t%d", s, &input_model);
    fscanf(fp, "%s\t%d", s, &output_model);
    fscanf(fp, "%s\t%d", s, &start_frame);
    fscanf(fp, "%s\t%d", s, &end_frame);
    fscanf(fp, "%s\t%d", s, &height);
    fscanf(fp, "%s\t%d", s, &width);
    fscanf(fp, "%s\t%lf", s, &square_width_diam_ratio);

    fclose(fp);

    bst_ptree::ptree tree;
    read_xml(Calibration_xml, tree);

    double diameter = tree.get<double>("RayCalibData.diameter");
    double rotation = tree.get<double>("RayCalibData.rotation");
    double radius = diameter * 0.5;
    cv::Point2d offset[3];
    cv::Point num;
    cv::Point2d center;
    cv::Point2d subGridRefPos[2][3];

    center.x = width * 0.5 + tree.get<double>("RayCalibData.offset.x");
    center.y = height * 0.5 - tree.get<double>("RayCalibData.offset.y");
    BOOST_FOREACH (const bst_ptree::ptree::value_type& child, tree.get_child("RayCalibData"))
        if (child.first == "lens_type") {
            offset[child.second.get<int>("<xmlattr>.id")].x = child.second.get<double>("offset.x");
            offset[child.second.get<int>("<xmlattr>.id")].y = child.second.get<double>("offset.y");
        }

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

    return {diameter, width, height, start_x, start_y, is_right_shift, is_horizontal,0.0,0.0, square_width_diam_ratio};
}
