#include "serial.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <list>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

using namespace cv;
using namespace std;

//constant of line color
const int WHITE = 255;
const int BLACK = 0;

//constant of image size
const int BINARY_IMAGE_WIDTH = 160;
const int BINARY_IMAGE_HEIGHT = 120;

//constant for LINE detection
const int LINE_WIDTH_THRESHOLD = 10;
const int STOP_LINE_THRESHOLD = BINARY_IMAGE_WIDTH * ((double)2/3);

//constant for control
const int SPEED = 100;  //motor base speed
const int TARGET = 30;  //target x px
const int KP =  1.7;    //coefficient for propotional control
const double STOP_TIME = 3.0;
const bool debug = true;

//constant for serial
const string SERIAL_PORT("/dev/ttyUSB0");  //Arduino uno device port
const speed_t BAUDRATE = B9600;     //baudrate to communicate with Arduino uno

//for debug
// #define __DEBUG__

//data type for LINE
struct LINE{
    int left_edge;
    int right_edge;
};

void binalizeImage(Mat& src_img, Mat& resized_img, Mat& dst_img);
int detectLines(Mat &image,int base_height,list<LINE>& line_list);
void updateState(Serial& ser, int total_white_pix, list<LINE>& line_list);

int main(int argc, char** argv){
    // open a serial port and a camera
    Serial ser(SERIAL_PORT);
    VideoCapture cap;
    cap.open(0);
    if(!cap.isOpened()){
	cout << "cant open camera." << endl;
        return -1;
    }
    // get start time
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();   

    while(1){
        // binalize image
        Mat src_img, resized_img, binalized_img;
        cap >> src_img;
        binalizeImage(src_img, resized_img, binalized_img);

        // calculate frame rate
        end = std::chrono::system_clock::now();
        double frame_rate = 1000. / (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << "frame rate: " << frame_rate << "fps" << endl;
        start = end;

        // count white pixels
        int total_white_pix = 0;
        list<LINE> center_line_list;
        total_white_pix = detectLines(binalized_img, binalized_img.rows/2, center_line_list);

        // update a state
        updateState(ser,total_white_pix,center_line_list);

        // show images
#ifdef __DEBUG__
	    if(showimg){
            line(resized_img, Point(0,80), Point(160,80), Scalar(0,200,0), 3, CV_AA);
	        imshow("resized_img", resized_img);
	        imshow("binalized_img", binalized_img);
	        waitKey(1);
	    }
#endif
    }

    return 0;
}

void binalizeImage(Mat &src_img, Mat& resized_img, Mat &dst_img){
    Mat gray_img;
    resize(
        src_img, resized_img,
        Size(),
        ((double)BINARY_IMAGE_WIDTH)/src_img.cols, ((double)BINARY_IMAGE_HEIGHT)/src_img.rows
    );
    cvtColor(resized_img, gray_img, CV_BGR2GRAY);
    threshold(
        gray_img, dst_img,
        160, WHITE, CV_THRESH_BINARY
    );
}

int detectLines(Mat &image,int base_height,list<LINE>& line_list){
    int left_edge = 0;
    int count = 0;
    int white_pix_count = 0;

    //Make line list.
    for(int x=0; x < image.cols; x++){
        //cout << "img_cols =" << x << endl;
        int pix_val = image.at<uchar>(base_height,x);
        white_pix_count += pix_val == WHITE ? 1 : 0;

        if(pix_val == WHITE){
            if(x!=0 || image.at<uchar>(base_height,x-1) == BLACK)
                left_edge = x;

            count ++;
        } else {
            //if line is enough thick, add to line list.
            if(count > LINE_WIDTH_THRESHOLD){
                LINE line = {left_edge,x-1};
                line_list.emplace_back(line);
            }
            count = 0;
        }
    }

    //If right terminal is black and line is thicker than threshold ,Add line to line list. 
    if(image.at<uchar>(base_height,image.cols-1) == WHITE && count > LINE_WIDTH_THRESHOLD){
        LINE line = {left_edge,image.cols-1};
        line_list.emplace_back(line);
    }
    return white_pix_count;
}

void updateState(Serial& ser, int total_white_pix, list<LINE>& line_list){
   
    if(STOP_LINE_THRESHOLD < total_white_pix){ 
        cout << "Stop line detected." << endl;

        // send a command to stop
        ser.sendRunCommand(0, 0);

        // stop while 3 second
        sleep(STOP_TIME);

        // send a command to run
        ser.sendRunCommand(70, 70);

    // spin if cannot find a line
    }else if(line_list.empty()){
        cout << "Line not found." << endl;

        // send a command to curve
        ser.sendRunCommand(100, 20);

    // curve with P control
    }else if(line_list.back().right_edge < BINARY_IMAGE_WIDTH/2){             
	    int diff_x_D = line_list.back().right_edge - TARGET; 
        cout << "x = " << line_list.back().right_edge<< " ";
        cout << "diff_x_D = " << diff_x_D << " ";
        int l_vel = SPEED + diff_x_D * KP;
        int r_vel = SPEED - diff_x_D * KP;
	    cout << "vel=" << l_vel << "," << r_vel <<" ";

        string command = "r," + to_string(r_vel) + "," + to_string(l_vel) + ";";
	    cout << "command " << command << endl; 
        ser.sendRunCommand(r_vel, l_vel);
    }
}

