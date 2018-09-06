/**
 * @file line_tracer.cpp
 * @brief Line trace programm.
 * @author Kumikomi
 * @date 05 Sep. 2018
 */

#include "serial.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <sstream>
#include <chrono>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

using namespace cv;
using namespace std;

//! motor base speed
const int SPEED = 100;
//! target x px
const int TARGET = 30;
//! coefficient for propotional control
const int KP =  1.7;    
const double STOP_TIME = 3.0;
//! Arduino uno device port
const string SERIAL_PORT("/dev/ttyUSB0");
//! baudrate to communicate with Arduino uno
const speed_t BAUDRATE = B9600;
const int WHITE = 255;
const int BLACK = 0;

void binalize_image(Mat &src_img, Mat &dst_img);
void count_white_pixels(Mat &image, int *ave_of_pix_val, int *edge_pix_idx);
void update_state(Serial ser, int ave_of_pix_val, int edge_pix_idx);

/*
 * @brief A function for autonomous driving.
 */
int main(int argc, char **argv){
    // open a serial port and a camera
    Serial ser(SERIAL_PORT);
    VideoCapture cap;
    cap.open(0);
    if(!cap.isOpened())
        return -1;
    
    // get start time
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();   

    while(1){
        // binalize image
        Mat src_img, binalized_img;
        cap >> src_img;
        binalize_image(src_img, binalized_img);

        // calculate frame rate
        end = std::chrono::system_clock::now();
        double frame_rate = 1. / (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << "frame rate: " << frame_rate << "fps" << endl;
        start = end;

        // count white pixels
        int ave_of_pix_val, edge_pix_idx;
        count_white_pixels(binalized_img, &ave_of_pix_val, &edge_pix_idx);

        // update a state
        update_state(ser, ave_of_pix_val, edge_pix_idx);

        // show images
        line(src_img, Point(0,80), Point(180,80), Scalar(0,200,0), 3, CV_AA);
        imshow("src_img", src_img);
        imshow("binalized_img", binalized_img);
        waitKey(1);
    }

    return 0;
}

/*
 * @brief This function binalizes an image
 * @param src_img[in] An input image
 * @param dst_img[out] An binalized image
 */
void binalize_image(Mat &src_img, Mat &dst_img){
    Mat gray_img, binalized_img;
    resize(
        src_img, src_img,
        Size(),
        180.0 / src_img.cols, 160.0 / src_img.rows
    );
    cvtColor(src_img, gray_img, CV_BGR2GRAY);
    threshold(
        gray_img, binalized_img,
        160, WHITE, CV_THRESH_BINARY
    );
}

/*
 * @brief This function counts while pixels on the center line.
 * @param image[in] An input image
 * @param *ave_of_pix_val[out] Average of pixels value on the center line
 * @param *edge_pix_idx[out] The edge of index of a pixel on the  center line
 */
void count_white_pixels(Mat &image, int *ave_of_pix_val, int *edge_pix_idx){
    int sum_of_pix_val = 0;
    int count = 0;

    for(int x = 0; x < image.cols; x++){
        int pix_val = image.at<uchar>(image.rows / 2, x);
        sum_of_pix_val += pix_val; 
        bool counting = false;

        if(!counting){
            if(pix_val == WHITE){
                counting = true;
                count++;
            }
        }else{
            if(pix_val == WHITE){
                count++;
            }else if(pix_val == BLACK && 10 < count){
                *edge_pix_idx = x;
                counting = false;
                cout << "x = " << x << endl;
            }else if(pix_val == BLACK && count <= 10){
                counting = false;
                count = 0;
            }
        }
    }

    *ave_of_pix_val = sum_of_pix_val / image.cols;
}

/*
 * @brief This function updates a state for runnning
 * @param ser[in] An object of Serial class
 * @param ave_of_pix_val[in] Average of pixels value on the center line
 * @param edge_pix_idx[in] The edge of index of a pixel on the  center line
 */
void update_state(Serial ser, int ave_of_pix_val, int edge_pix_idx){

    // stop a front of stop line
    if(165 < ave_of_pix_val){ 
        cout << "Stop line detected." << endl;

        // send a command to stop
        string command("r,0,0;");
        ser.write_command(command);

        // stop while 3 second
        sleep(STOP_TIME);

        // send a command to run
        command = string("r,70,70;");
        ser.write_command(command);

    // curve if cannot find a line
    }else if(ave_of_pix_val == 0 || edge_pix_idx == 0){
        cout << "Line not found." << endl;

        // send a command to curve
        string command("r,100,20;");
        ser.write_command(command);

    // curve with P control
    }else if(edge_pix_idx < 80){              
        cout << "x = " << edge_pix_idx << endl;
        cout << "diff_x_D = " << edge_pix_idx << endl;
        int l_vel = SPEED + (edge_pix_idx - TARGET) * KP;
        int r_vel = SPEED - (edge_pix_idx - TARGET) * KP;
        string command = string("r,") + to_string(r_vel) + string(",") + to_string(l_vel) + string(";");
        ser.write_command(command);
    }
}

