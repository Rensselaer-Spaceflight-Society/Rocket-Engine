#ifndef COMMAND_H
#define COMMAND_H

#include <QString>

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
