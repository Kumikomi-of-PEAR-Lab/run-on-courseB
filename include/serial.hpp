/**
 * @file serial.h
 * @brief Serial communication module for Linux to send commands to mbot
 * @author Kenta Arai
 * @date 05 Sep. 2018
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <string>

#include <termios.h>

/*
 * @class Serial
 * @brief A class for serial communication to send commands to mbot
 */
class Serial{
    private:
        const speed_t BAUDRATE = B9600;
        int fd;
    public:
        Serial(std::string port_name);
        ~Serial();
        void write_command(std::string command);
};

#endif
