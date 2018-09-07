#ifndef _DETECT_COLOR_HPP_
#define _DETECT_COLOR_HPP_

#include <opencv2/opencv.hpp>

class DetectColor{
    public:
        DetectColor(int lower = 0, int upper = 255);
        bool detectRed(cv::Mat &image);
    private:
        cv::Scalar lower1;
        cv::Scalar upper1;
        cv::Scalar lower2;
        cv::Scalar upper2;
};

#endif  // _DETECT_COLOR_HPP_
