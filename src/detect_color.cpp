#include "detect_color.hpp"

#include <iostream>

DetectColor::DetectColor(int lower, int upper){
	std::cout << "lower: " << lower << std::endl;
	std::cout << "upper: " << upper << std::endl;

    cv::Scalar lower1 = cv::Scalar(0,   128, lower);
    cv::Scalar upper1 = cv::Scalar(30,  255, upper);
    cv::Scalar lower2 = cv::Scalar(150, 127, lower);
    cv::Scalar upper2 = cv::Scalar(179, 255, upper);

	std::cout << lower1 << std::endl;
	std::cout << upper1 << std::endl;
	std::cout << lower2 << std::endl;
	std::cout << upper2 << std::endl;
}

bool DetectColor::detectRed(cv::Mat &image){
    // convert image to HSV format
    cv::Mat hsv;
    cv::cvtColor(image, hsv, CV_BGR2HSV);
   
    // generate mask
    cv::Mat mask1, mask2;
    cv::inRange(hsv, lower1, upper1, mask1);
    cv::inRange(hsv, lower2, upper2, mask2);
 
    cv::imshow("hoge", mask2);
    cv::waitKey(1);

    // mask
    cv::Mat masked;
    image.copyTo(masked, mask1 + mask2);

    // remove noise from a gray scale image
    cv::Mat channels[3], opened, closed;
    cv::split(masked, channels);
    cv::morphologyEx(channels[2], opened, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(opened, closed, cv::MORPH_CLOSE, cv::Mat());

    return true;
}


