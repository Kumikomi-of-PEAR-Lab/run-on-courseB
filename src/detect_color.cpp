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

    // mask
    cv::Mat masked1, masked2;
    image.copyTo(masked1, mask1);
    image.copyTo(masked2, mask2);

    // remove noise from a gray scale image
    cv::Mat channels[3], opened1, closed1;
    // masked1
    cv::split(masked1, channels);
    cv::morphologyEx(channels[2], opened1, cv::MORPH_OPEN, cv::Mat::ones(3, 3, CV_8U));
    cv::imwrite("opened1_0.jpg", opened1);
    cv::morphologyEx(opened1, closed1, cv::MORPH_CLOSE, cv::Mat::ones(3, 3, CV_8U));
    cv::imwrite("closed1_0.jpg", closed1);
    for (int i = 0; i < 3; i++) {
        cv::morphologyEx(closed1, opened1, cv::MORPH_OPEN, cv::Mat::ones(i * 2 + 5, i * 2 + 5, CV_8U));
        cv::imwrite(std::string("opened1_") + std::to_string(i + 1) + std::string(".jpg"), closed1);
        cv::morphologyEx(opened1, closed1, cv::MORPH_CLOSE, cv::Mat::ones(i * 2 + 5, i * 2 + 5, CV_8U));
        cv::imwrite(std::string("closed1_") + std::to_string(i + 1) + std::string(".jpg"), closed1);
    }

    // masked1
    cv::Mat opened2, closed2;
    cv::split(masked2, channels);
    cv::morphologyEx(channels[2], opened2, cv::MORPH_OPEN, cv::Mat::ones(3, 3, CV_8U));
    cv::imwrite("opened2_0.jpg", opened2);
    cv::morphologyEx(opened2, closed2, cv::MORPH_CLOSE, cv::Mat::ones(3, 3, CV_8U));
    cv::imwrite("closed2_0.jpg", closed2);
    for (int i = 0; i < 3; i++) {
        cv::morphologyEx(closed2, opened2, cv::MORPH_OPEN, cv::Mat::ones(i * 2 + 5, i * 2 + 5, CV_8U));
        cv::imwrite(std::string("opened2_") + std::to_string(i + 1) + std::string(".jpg"), closed2);
        cv::morphologyEx(opened2, closed2, cv::MORPH_CLOSE, cv::Mat::ones(i * 2 + 5, i * 2 + 5, CV_8U));
        cv::imwrite(std::string("closed2_") + std::to_string(i + 1) + std::string(".jpg"), closed2);
    }

    // concatenate
    cv::Mat concat;
    cv::bitwise_and(closed1, closed2, concat);
    cv::imwrite("concat.jpg", concat);

    cv::imwrite("hsv.jpg", hsv);
    cv::imwrite("mask1.jpg", mask1);
    cv::imwrite("mask2.jpg", mask2);

    return true;
}

