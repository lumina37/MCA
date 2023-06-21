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
#include "LVC/preprocess/backward.hpp"
#include "LVC/preprocess/forward.hpp"

using namespace std;
using namespace boost::property_tree;
using namespace cv;

constexpr int IMAGE_SIZE[2][2] = {{3840, 2160}, {2048, 2048}};
constexpr double PI = 3.14159265;

// argv: filename; size_type
int main(int argc, char* argv[])
{
    if (argc != 3) {
        cout << "command line error, please provide correct parameters " << endl;
        exit(0);
    }
    string imageName = argv[1];
    int sizeType = stoi(argv[2]);

    string rawImagePath = "/mnt/e/Projects/task2-raytrix/experiment/data/RawImages/" + imageName + ".png";
    string preprocessedImagePath = "/mnt/e/Projects/task2-raytrix/experiment/data/PreprocessedImages/" + imageName + ".png";
    string inverseTransedImagePath = "/mnt/e/Projects/task2-raytrix/experiment/data/InverseTransedImages/" + imageName + ".png";
    string calibrationFile =
        "/mnt/e/Projects/task2-raytrix/experiment/data/CalibrationFiles/" + imageName + "_Calibration.xml";

    cv::Mat rawImage, preprocessedImage, inverseTransedImage;
    rawImage = cv::imread(rawImagePath);
    

    ptree tree;
    read_xml(calibrationFile, tree);
    int rows = IMAGE_SIZE[sizeType][1], cols = IMAGE_SIZE[sizeType][0];

    double diameter = tree.get<double>("RayCalibData.diameter");
    double rotation = tree.get<double>("RayCalibData.rotation");
    double radius = diameter * 0.5;
    cv::Point2d offset[3];
    cv::Point num;
    cv::Point2d center;
    cv::Point2d subGridRefPos[2][3];

    center.x = cols * 0.5 + tree.get<double>("RayCalibData.offset.x");
    center.y = rows * 0.5 - tree.get<double>("RayCalibData.offset.y");
    BOOST_FOREACH (const ptree::value_type& child, tree.get_child("RayCalibData"))
        if (child.first == "lens_type") {
            offset[child.second.get<int>("<xmlattr>.id")].x = child.second.get<float>("offset.x");
            offset[child.second.get<int>("<xmlattr>.id")].y = child.second.get<float>("offset.y");
        }

    // transpose each matrix
    if (rotation > PI / 4.0) {
        Mat tempImage;
        transpose(rawImage, tempImage);
        tempImage.copyTo(rawImage);
        center = Point2d(center.y, center.x);
        for (int i = 0; i < 3; ++i) {
            offset[i] = Point2d(offset[i].y, offset[i].x);
        }
	}

    //Calculation Number of Micro Lens
	int upNum = (int)((center.y - radius) / (radius * sqrt(3)));  
	int downNum = (int)((rows - (center.y + radius)) / (radius * sqrt(3)));
	num.y = upNum + 1 + downNum;
	double leftNum = (center.x - radius) / diameter;
	double rightNum = (cols - (center.x + radius)) / diameter;

	if (((leftNum - (int)leftNum) > 0.5) || ((rightNum - (int)rightNum) >= 0.5))
		num.x = (int)leftNum + 1 + (int)rightNum;
	else
		num.x = (int)leftNum + (int)rightNum;
	num.x = (int)(num.x / 3);
	
	//Calculation subGridRefPos
	for (int type = 0; type < 3; type++)
	{
		double refPos = center.x + diameter * -(offset[type].x + offset[type].y);	//On the assumption that either offset[].x or offset[].y is Zero.
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

    Point2d shift[2];
	shift[0].x = min(subGridRefPos[0][0].x, min(subGridRefPos[0][1].x, subGridRefPos[0][2].x));
	shift[0].y = min(subGridRefPos[0][0].y, min(subGridRefPos[0][1].y, subGridRefPos[0][2].y));
	shift[1].x = min(subGridRefPos[1][0].x, min(subGridRefPos[1][1].x, subGridRefPos[1][2].x));
	shift[1].y = min(subGridRefPos[1][0].y, min(subGridRefPos[1][1].y, subGridRefPos[1][2].y));
    float start_x = shift[0].x;
    float start_y = shift[0].y;
    bool is_right_shift = shift[0].x < shift[1].x;
    bool is_horizontal = rotation <= PI / 4.0;

    if (rotation > PI / 4.0) {
        Mat tempImage;
        transpose(rawImage, tempImage);
        tempImage.copyTo(rawImage);
        center = Point2d(center.y, center.x);
        float tmp = start_x;
        start_x = start_y;
        start_y = tmp;
	}

    lvc::Config cfg = {diameter, cols, rows, start_x, start_y, is_right_shift, is_horizontal};

    // cfg = {23.202295303345f, 2048, 2048, 12.2f, 12.125f, true, false};

    lvc::preprocessForward(cfg, rawImage, preprocessedImage);
    // Mat tempImage;
    // transpose(preprocessedImage, tempImage);
    // tempImage.copyTo(preprocessedImage);
    cv::imwrite(preprocessedImagePath, preprocessedImage);
    lvc::preprocessBackward(cfg, preprocessedImage, inverseTransedImage);
    cv::imwrite(inverseTransedImagePath, inverseTransedImage); 

    return 0;
}
