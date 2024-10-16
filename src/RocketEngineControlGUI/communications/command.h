#ifndef COMMAND_H
#define COMMAND_H

#include <QString>

#define TIME_PER_PING_MS 50
#define BYTES_IN_COMMAND 8
#define PING_COMMAND "PingPong"
#define SHUTDOWN_COMMAND "ShutDown"


enum class CommandPriority: u_int8_t {
    LOW,
    NORMAL,
    HIGH
};

class Command {
public:
    Command(QString data, CommandPriority priority = CommandPriority::LOW):
        commandString(data),
        priority(priority)
    {};

    bool operator<(const Command & other) const
    {
        return static_cast<u_int8_t>(this->priority) <
                    static_cast<u_int8_t>(other.priority);
    }

    bool operator==(const std::string & input) const
    {
        return this->commandString.toStdString() == input;
    }

    CommandPriority getPriority() const { return priority; }
    const QString & getString() const { return commandString; }

private:
    QString commandString;
    CommandPriority priority;

};

#endif // COMMAND_H
