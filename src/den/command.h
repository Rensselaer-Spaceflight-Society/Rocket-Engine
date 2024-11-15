#ifndef COMMAND_H
#define COMMAND_H
#include <QString>

class command
{
public:
    command(QString cmd, int pt) : Command(cmd), priority(pt) {}
    QString getCommand();
    int getPriority();

private:
    QString Command;
    int priority;
};

#endif // COMMAND_H
