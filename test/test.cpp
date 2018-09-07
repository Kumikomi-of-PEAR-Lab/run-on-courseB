#include "detect_color.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>

int main(void){
    cv::VideoCapture cap(0);

    if(!cap.isOpened()){
        std::cout << "Cannot find a camera." << std::endl;
        return -1;
    }

    DetectColor dc;

    cv::Mat img;
    while(1){
        cap >> img;
        dc.detectRed(img);
    }

    return 0;
}

