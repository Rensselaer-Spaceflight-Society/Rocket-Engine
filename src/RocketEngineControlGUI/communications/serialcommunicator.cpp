#include "serialcommunicator.h"

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

SerialCommunicator::SerialCommunicator(const char *port, speed_t baudrate)
{
    // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

    strcpy(this->tty_port, port);
    this->comms_baudrate = baudrate;

    // Open the serial port
    this->fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);

    if (this->fd < 0)
    {
        throw std::runtime_error("Error " + std::to_string(errno) + " opening " + port + ": " + strerror(errno));
    }

    try{
        this->setup_port();
    } catch (std::exception &e) {
        close(this->fd);
        throw e;
    }    
}

void SerialCommunicator::setup_port() {
    // Get current serial port settings
    memset(&this->tty, 0, sizeof(this->tty));

    if (tcgetattr(this->fd, &this->tty) != 0)
    {
        std::cerr << "Error " << errno << " unable to get old tty config from tcgetattr: " << strerror(errno) << std::endl;
    }

    // Save old serial port settings
    this->tty_old = this->tty;

    // Setting Control Modes
    this->tty.c_cflag &= (tcflag_t)~PARENB;        // Clear parity bit, disabling parity (most common)
    this->tty.c_cflag &= (tcflag_t)~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
    this->tty.c_cflag &= (tcflag_t)~CSIZE;         // Clear all the size bits
    this->tty.c_cflag |= (tcflag_t)CS8;            // 8 bits per byte (most common)
    this->tty.c_cflag &= (tcflag_t)~CRTSCTS;       // Disable RTS/CTS hardware flow control to ensure no data is lost
    this->tty.c_cflag |= (tcflag_t)CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    // Setting Local Modes
    this->tty.c_lflag &= (tcflag_t)~ICANON; // Disable canonical mode to disable line buffering
    this->tty.c_lflag &= (tcflag_t)~ECHO;   // Disable echo
    this->tty.c_lflag &= (tcflag_t)~ECHOE;  // Disable erasure
    this->tty.c_lflag &= (tcflag_t)~ECHONL; // Disable new-line echo
    this->tty.c_lflag &= (tcflag_t)~ISIG;   // Disable interpretation of INTR, QUIT and SUSP

    // Setting Input Modes
    this->tty.c_iflag &= static_cast<tcflag_t>(~(IXON | IXOFF | IXANY));                                      // Turn off s/w flow ctrl
    this->tty.c_iflag &= static_cast<tcflag_t>(~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL)); // Disable any special handling of received bytes

    // Setting Output Modes
    this->tty.c_oflag &= (tcflag_t)~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    this->tty.c_oflag &= (tcflag_t)~ONLCR; // Prevent conversion of newline to carriage return/line feed

    // VMIN and VTIME determine how read() behaves
    this->tty.c_cc[VMIN] = 0;  // 0 means it will return immediately
    this->tty.c_cc[VTIME] = 0; // Return immediately if there is no data to read

    // Set in/out baud rate
    cfsetspeed(&this->tty, comms_baudrate);

    // Save tty settings, also checking for error
    if (tcsetattr(this->fd, TCSANOW, &this->tty) != 0)
    {
        throw std::runtime_error("Error " + std::to_string(errno) + " from tcsetattr: " + strerror(errno));
    }
}

SerialCommunicator::~SerialCommunicator()
{
    close(this->fd);
}

void SerialCommunicator::write_data(const char *data)
{
    write(fd, data, strlen(data));
}

int SerialCommunicator::read_data(char *&buffer, size_t size)
{
    return read(fd, buffer, size);
}
