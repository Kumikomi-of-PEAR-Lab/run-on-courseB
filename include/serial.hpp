#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <string>

#include <termios.h>

class Serial{
    private:
        const speed_t BAUDRATE = B9600;
        int fd;
    public:
        Serial(std::string port_name);
        ~Serial();
        void sendRunCommand(int right_power, int left_power);
    private:
        void writeString(std::string str);
};

#endif
