#ifndef __SERIAL_COMMUNICATOR_H__
#define __SERIAL_COMMUNICATOR_H__

#include "data_io.hpp"
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

class SerialCommunicator : public DataIO
{
private:
    int fd;
    char tty_port[255];
    speed_t comms_baudrate;
    struct termios tty;
    struct termios tty_old;
    bool is_open;

    void setup_port();

public:
    SerialCommunicator(const char *port, speed_t baudrate);
    ~SerialCommunicator();

    void write_data(const char *data) override;
    ssize_t read_data(char *&buffer, size_t size) override;

    bool isOpen() { return this->is_open; }
};

#endif // __SERIAL_COMMUNICATOR_H__