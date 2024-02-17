#ifndef DATA_IO_H
#define DATA_IO_H

#include <cstddef>

class DataIO
{
public:
    virtual void write_data(const char *data) = 0;
    virtual ssize_t read_data(char* &buffer, size_t size) = 0;

    virtual ~DataIO() {}
};


#endif // DATA_IO_H
