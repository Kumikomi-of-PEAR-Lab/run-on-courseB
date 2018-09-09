#include "detect_color.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv){
    if(argc < 3){
        std::cout << "Usage: " << argv[0] << "src_file dst_file" << std::endl;
        exit(1);
    }
    // cv::VideoCapture cap(0);

    // if(!cap.isOpened()){
    //     std::cout << "Cannot find a camera." << std::endl;
    //     return -1;
    // }

    DetectColor dc(228, 255);

    cv::Mat img = cv::imread(argv[1], 1);
    dc.detectRed(img);

    return 0;
}

