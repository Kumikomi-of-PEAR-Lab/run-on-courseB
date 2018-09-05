#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <chrono>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

const int SPEED = 100;	//motor base speed
const int TARGET = 30;	//target x px
const int KP =  1.7;	//coefficient for propotional control
const char SERIAL_PORT[] = "/dev/ttyUSB0";	//Arduino uno device port
const speed_t BAUDRATE = B9600;		//baudrate to communicate with Arduino uno

int initSerial(struct termios&);
void write_data(int,int,int,char);
void timer(double);


int main(int argc, char *argv[])
{
	int fd;
	double set_time;
	char buf[256];
	char l_buf[5];
	char r_buf[5];
	struct termios tio;

	Mat src;
        Mat src_g, src_b;
	int y, l_vel, r_vel;
	int b_px;
	int prev_x = 0;
	
        VideoCapture cap;

	std::chrono::system_clock::time_point start, end;

	//initialize serial port
	fd = initSerial(tio);
	if(fd){
		cout << "Couldn't open serial port."<<endl;
		cout << "LINE:" << __LINE__ <<endl;
		return -1;
	}		

	//open camera device
        cap.open(0);
	if(!cap.isOpened()) return -1;

	//set start time
	start = std::chrono::system_clock::now();	
	
	while(1){
		int sum_px = 0, avg_px = 0, edg_px =0;
		int rate = 0;
		int count = 0;
		int line_num = 0;
		int flag = false;
		double s,e;
		double total_time = 0.0;
	 
		//image comversion
		cap >> src;
		resize(src,src,Size(),180.0/src.cols,160.0/src.rows);
		cvtColor(src, src_g, CV_BGR2GRAY);
		threshold(src_g, src_b, 160, WHITE, CV_THRESH_BINARY);
		y = src.rows/2;	
		
		//calcration of flame rate
		rate++;
		end  = std::chrono::system_clock::now();
		if((int)std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() > 1000){
			start = end;
			rate = 0;
		}

		memset(buf,0, sizeof(buf));

		for(int x = 0;x < src.cols; x++){
			b_px = src_b.at<uchar>(y,x);
			sum_px += b_px; 
			if(b_px == WHITE && flag == false){
				flag = true;
				count++;
			}else if(b_px == WHITE && flag == true){
				count++;
			}else if(b_px == 0 && flag == true && 10 < count){
				line_num++;
				edg_px = x;
				flag = false;
				printf(" x = %d\n", x);
			}else if(b_px == 0 && flag == true && count <= 10){
				flag = false;
				count = 0;
			}
		}
			
		avg_px = sum_px/src.cols;
		//printf("avg_px = %d\n",avg_px);

		if(165 < avg_px ){ 
			printf("stop line detected.\n");
			strcat (buf,"r,0,0;");
			write(fd,buf,strlen(buf));
			set_time = 3.0;
			timer(set_time);

			strcat(buf,"r,70,70;");
			write(fd,buf,strlen(buf));
			set_time = 1.0;
			timer(set_time);
			cap.open(0);
			
		}else if(avg_px == 0 || edg_px == 0){
			printf("Line not found.\n");
			strcat (buf,"r,100,20;");
			write(fd,buf,strlen(buf));
		}else if(edg_px < 80){			    
			printf("x = %d\n", edg_px);
			printf("diff_x_D = %d\n", edg_px - prev_x);
			l_vel = SPEED + (edg_px - TARGET)*KP;
			r_vel = SPEED - (edg_px - TARGET)*KP;
			prev_x = edg_px;	
			write_data(l_vel,r_vel,fd,buf);
			write(fd,buf,strlen(buf));
		}
		
		line(src, Point(0,80), Point(180,80), Scalar(0,200,0), 3, CV_AA);
		imshow("src", src);
		imshow("binary", src_b);
		waitKey(1);
	}
	close(fd);
        return 0;
}

//initialization of serial port
int initSerial(struct termios& tio){
	int fd = open(SERIAL_PORT, O_RDWR);
	if(fd < 0) return 0;
	
	tio.c_cflag += CREAD;
	tio.c_cflag += CLOCAL;
	tio.c_cflag += CS8; 
    	tio.c_cflag += 0; 
    	tio.c_cflag += 0;

    	cfsetispeed( &tio, BAUDRATE );
	cfsetospeed( &tio, BAUDRATE );
    	cfmakeraw(&tio);      
    	tcsetattr( fd, TCSANOW, &tio ); 
    	ioctl(fd, TCSETS, &tio);
 	return fd;
}

void write_data(int l_vel, int r_vel, int fd, char *buf){
	char l_buf[5];
	char r_buf[5];
	printf("left = %d\n",l_vel);
	printf("right = %d\n",r_vel);
	sprintf(l_buf,"%d",l_vel);
	sprintf(r_buf,"%d",r_vel);
	strcat(buf,"r,");
	strcat(buf,r_buf);
	strcat(buf,",");	
	strcat(buf,l_buf);
	strcat(buf,";");
}

void timer(double set_time){
	double s,e;
	double total_time = 0.0;
	s = clock()/CLOCKS_PER_SEC;
	while(1){
	if(total_time > set_time)
	break;
	e = clock()/CLOCKS_PER_SEC;
	total_time = e - s;
	}
}

//detect white lines on base line
void detectLines(Mat src,int base_height){
	const int WHITE = 255;
	const int BLACK = 0;

	for(int x = 0;x < src.cols; x++){
		//get pix value
		b_px = src.at<uchar>(base_height,x);
		sum_px += b_px == WHITE ? 1 : 0; 


		if(b_px == WHITE)
			 count++;
		
		else if(x != 0 && src.at<uchar>(base_height,x-1) == WHITE){
			if(count > 10){
				line_num++;
				edg_px = x;
				printf(" x = %d\n", x);
			} 
			count = 0;
		}
	}
}
