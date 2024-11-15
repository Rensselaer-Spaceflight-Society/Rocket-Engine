#include "command.h"

int command::getPriority(){
    return this->priority;
}

QString command::getCommand(){
    return this->Command;
}

