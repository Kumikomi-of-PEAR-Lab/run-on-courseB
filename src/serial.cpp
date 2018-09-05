#include "serial.hpp"

#include <iostream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

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

Serial::~Serial(){
    std::cout << "Close the serial port" << std::endl;
    close(fd);
}

void Serial::write_command(std::string command){
    write(fd, command.c_str(), command.size());
}

