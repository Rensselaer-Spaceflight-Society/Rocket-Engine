#include "UARTCommunicator.hpp"

#include <string>
#include <string.h>
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()
#include <exception>
#include <stdexcept>

UARTCommunicator::UARTCommunicator(
    const std::string &transmit_port,
    const std::string &receive_port,
    int baud_rate)
    : transmit_port(transmit_port),
      receive_port(receive_port),
      baud_rate(baud_rate)
{
    transmit_fd = open(transmit_port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (transmit_fd < 0)
    {
        throw std::runtime_error("Error " + std::to_string(errno) + " opening " + transmit_port + ": " + strerror(errno));
    }

    receive_fd = open(receive_port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (receive_fd < 0)
    {
        throw std::runtime_error("Error " + std::to_string(errno) + " opening " + receive_port + ": " + strerror(errno));
    }

    try {
        setup_port(transmit_fd, transmit_tty, transmit_tty_old);
    } catch (std::exception &e) {
        close(transmit_fd);
        throw e;
    }

    try {
        setup_port(receive_fd, receive_tty, receive_tty_old);
    } catch (std::exception &e) {
        close(receive_fd);
        throw e;
    }

}

void UARTCommunicator::setup_port(int fd, struct termios &tty, struct termios &tty_old)
{
    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(fd, &tty) != 0)
    {
        throw std::runtime_error("Error " + std::to_string(errno) + " from tcgetattr: " + strerror(errno));
    }

    tty_old = tty;

    tty.c_cflag &= (tcflag_t)~PARENB;        // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= (tcflag_t)~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= (tcflag_t)~CSIZE;         // Clear all the size bits
    tty.c_cflag |= (tcflag_t)CS8;            // 8 bits per byte (most common)
    tty.c_cflag &= (tcflag_t)~CRTSCTS;       // Disable RTS/CTS hardware flow control to ensure no data is lost
    tty.c_cflag |= (tcflag_t)CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= (tcflag_t)~ICANON; // Disable canonical mode to disable line buffering
    tty.c_lflag &= (tcflag_t)~ECHO;   // Disable echo
    tty.c_lflag &= (tcflag_t)~ECHOE;  // Disable erasure
    tty.c_lflag &= (tcflag_t)~ECHONL; // Disable new-line echo
    tty.c_lflag &= (tcflag_t)~ISIG;   // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= static_cast<tcflag_t>(~(IXON | IXOFF | IXANY));                                      // Turn off s/w flow ctrl
    tty.c_iflag &= static_cast<tcflag_t>(~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL)); // Disable any special handling of received bytes

    tty.c_oflag &= (tcflag_t)~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= (tcflag_t)~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VMIN] = 0;  // 0 means it will return immediately
    tty.c_cc[VTIME] = 0; // Return immediately if there is no data to read

    cfsetspeed(&tty, baud_rate);

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        throw std::runtime_error("Error " + std::to_string(errno) + " from tcsetattr: " + strerror(errno));
    }
}

UARTCommunicator::~UARTCommunicator()
{
    close(transmit_fd);
    close(receive_fd);
}

void UARTCommunicator::write_data(const char *data)
{
    write(transmit_fd, data, strlen(data));
}

ssize_t UARTCommunicator::read_data(char* &buffer, size_t size)
{
    return read(receive_fd, buffer, size);
}
