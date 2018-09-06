/**
 * @file serial.cpp
 * @brief Serial communication module for Linux to send commands to mbot
 * @author Kenta Arai
 * @date 05 Sep. 2018
 */

#include "serial.hpp"

#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * @brief A constructor for Class Serial. This opens a serial port.
 * @param port_name[in] a port name used to serial communication
 */
Serial::Serial(std::string port_name){
    // open the port "port_name"
    fd = open(port_name.c_str(), O_RDWR);

    // check an error
    if(fd < 0){
        std::cout << "Cannot open the port\"";
        std::cout << port_name << "\"" << std::endl;
        exit(1);
    }else{
        std::cout << "Open the port\"";
        std::cout << port_name << "\"" << std::endl;
    }

    // settings
    struct termios tio;
    tio.c_cflag = CREAD | CLOCAL | CS8;

    cfsetispeed(&tio, BAUDRATE);
	cfsetospeed(&tio, BAUDRATE);
    cfmakeraw(&tio);      
    tcsetattr(fd, TCSANOW, &tio); 
    ioctl(fd, TCSETS, &tio);
}

/*
 * @brief A destructor for Class Serial. This closes the serial port.
 */
Serial::~Serial(){
    std::cout << "Close the serial port" << std::endl;
    close(fd);
}

/* 
 * @brief A function to send a command.
 * @param[in] an command
 */
void Serial::write_command(std::string command){
    write(fd, command.c_str(), command.size());
}

