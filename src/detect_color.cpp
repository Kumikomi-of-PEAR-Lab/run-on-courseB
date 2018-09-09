#include "detect_color.hpp"

#include <iostream>

DetectColor::DetectColor(int lower, int upper){
    lower1 = cv::Scalar(0,   1, lower);
    upper1 = cv::Scalar(30,  255, upper);
    lower2 = cv::Scalar(150, 1, lower);
    upper2 = cv::Scalar(179, 255, upper);

	std::cout << "lower1: " << lower1 << std::endl;
	std::cout << "upper1: " << upper1 << std::endl;
	std::cout << "lower2: " << lower2 << std::endl;
	std::cout << "upper2: " << upper2 << std::endl;
}

bool DetectColor::detectRed(cv::Mat &image){
    // convert image to HSV format
    cv::Mat hsv;
    cv::cvtColor(image, hsv, CV_BGR2HSV);
   
    // generate mask
    cv::Mat mask1, mask2, mask;
    cv::inRange(hsv, lower1, upper1, mask1);
    cv::inRange(hsv, lower2, upper2, mask2);
    cv::bitwise_and(mask1, mask2, mask);

    // cv::imshow("hoge", mask2);
    // cv::waitKey(1);

    // mask
    cv::Mat masked;
    image.copyTo(masked, mask);

    // remove noise from a gray scale image
    cv::Mat channels[3], opened, closed;
    cv::split(masked, channels);
    cv::morphologyEx(channels[2], closed, cv::MORPH_CLOSE, cv::Mat::ones(7, 7, CV_8U));
    cv::morphologyEx(closed, opened, cv::MORPH_OPEN, cv::Mat::ones(5, 5, CV_8U));

    cv::imwrite("hsv.jpg", hsv);
    cv::imwrite("mask1.jpg", mask1);
    cv::imwrite("mask2.jpg", mask2);
    cv::imwrite("mask.jpg", mask);
    cv::imwrite("masked.jpg", masked);
    cv::imwrite("opened.jpg", opened);
    cv::imwrite("closed.jpg", closed);

    return true;
}

