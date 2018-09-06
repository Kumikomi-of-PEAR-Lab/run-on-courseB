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

//constant for serial
const string SERIAL_PORT("/dev/ttyUSB0");  //Arduino uno device port
const speed_t BAUDRATE = B9600;     //baudrate to communicate with Arduino uno

//data type for LINE
struct LINE{
  int left_edge,
  int right_edge
};

void binalizeImage(Mat &src_img, Mat &dst_img);
int detectLine(Mat&,int,list<Line>&);
void updateState(Serial,int,lint<line>&);
//void count_white_pixels(Mat &image, int *ave_of_pix_val, int *edge_pix_idx);
//void update_state(Serial ser, int ave_of_pix_val, int edge_pix_idx);

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
        int total_white_pix;
        list<LINE> center_line_list;
        total_white_pix = detectLine(binalized_img,image.rows / 2,center_line_list);
        //int ave_of_pix_val, edge_pix_idx;
        //count_white_pixels(binalized_img, &ave_of_pix_val, &edge_pix_idx);

        // update a state
        updateState(ser,center_line_list);
        //update_state(ser, ave_of_pix_val, edge_pix_idx);

        // show images
        line(src_img, Point(0,80), Point(180,80), Scalar(0,200,0), 3, CV_AA);
        imshow("src_img", src_img);
        imshow("binalized_img", binalized_img);
        waitKey(1);
    }

    return 0;
}

void binalizeImage(Mat &src_img, Mat &dst_img){
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

int detectLines(Mat &image,int base_height,list<LINE>& line_list){
  int left_edge = 0;
  
  //Make line list.
  for(int x=0; x < src.cols; x++){
    pix_val = image.at<uchar>(base_height,x);
    white_pix_count = pix_val == WHITE ? 1 : 0;
    
    if(pix_val == WHITE){
      if(x!=0 || src.at<uchar>(base_height,x-1) == BLACK)
        left_edge = x;

      count ++;
    }
    else {
      //if line is enough thick, add to line list.
      if(count > LINE_WIDTH_THRESHOLD){
        LINE line = {left_edge,x-1};
        line_list.emplace_back(line);
      }
      count = 0;
    }
  }

  //If right terminal is black and line is thicker than threshold ,Add line to line list. 
  if(src.at<uchar>(base_height,x) == WHITE && count > LINE_WIDTH_THRESHOLD){
    LINE line = {left_edge,x-1};
    line_list.emplace_back(line);
  }
  return white_pix_count;
}

void updatState(Serial ser, int total_white_pix, list<Line>& line_list){
    
    if(STOP_LINE_THRESHOLD < total_white_pix){ 
        cout << "Stop line detected." << endl;

        // send a command to stop
        string command("r,0,0;");
        ser.write_command(command);

        // stop while 3 second
        sleep(STOP_TIME);

        // send a command to run
        command = string("r,70,70;");
        ser.write_command(command);

    // spin if cannot find a line
    }else if(line_list.empty()){
        cout << "Line not found." << endl;

        // send a command to curve
        string command("r,100,20;");
        ser.write_command(command);

    // curve with P control
    }else if(line_list.back().right_edge < 80){              
        cout << "x = " << edge_pix_idx << endl;
        cout << "diff_x_D = " << edge_pix_idx << endl;
        int l_vel = SPEED + (edge_pix_idx - TARGET) * KP;
        int r_vel = SPEED - (edge_pix_idx - TARGET) * KP;
        string command = string("r,") + to_string(r_vel) + string(",") + to_string(l_vel) + string(";");
        ser.write_command(command);
    }
}
/*
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

    // spin if cannot find a line
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
*/
