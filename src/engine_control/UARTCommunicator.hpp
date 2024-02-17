#ifndef UART_COMMUNICATOR_H
#define UART_COMMUNICATOR_H

#include <string>
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()
#include "data_io.hpp"

class UARTCommunicator : public DataIO
{
private:
    std::string transmit_port;
    std::string receive_port;
    int baud_rate;

    int transmit_fd;
    int receive_fd;

    struct termios transmit_tty;
    struct termios receive_tty;
    struct termios transmit_tty_old;
    struct termios receive_tty_old;

    void setup_port(int fd, struct termios &tty, struct termios &tty_old);

public:
    UARTCommunicator(
        const std::string &transmit_port,
        const std::string &receive_port,
        int baud_rate);

    void write_data(const char *data) override;
    ssize_t read_data(char* &buffer, size_t size) override;

    ~UARTCommunicator();
};

#endif // UART_COMMUNICATOR_H