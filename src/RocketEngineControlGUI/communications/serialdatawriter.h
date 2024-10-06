#ifndef SERIALDATAWRITER_H
#define SERIALDATAWRITER_H

#include <QThread>

enum class CommandPriority: u_int8_t {
    LOW,
    NORMAL,
    HIGH
};

class Command {
public:
    Command(QString data, CommandPriority priority):
        commandString(data),
        priority(priority)
    {};

    bool operator<(const Command & other) const;

    CommandPriority getPriority() const { return priority; }
    const QString & getString() const { return commandString; }

private:
    QString commandString;
    CommandPriority priority;

};

class SerialDataWriter : public QThread
{
public:
    explicit SerialDataWriter(QObject *parent = nullptr);
};

#endif // SERIALDATAWRITER_H
