#include "serialdatawriter.h"

SerialDataWriter::SerialDataWriter(QSerialPort * tx, QObject *parent)
    : QThread{parent}, serialPort(tx)
{
    this->commandQueue = new std::priority_queue<Command>();
}

SerialDataWriter::~SerialDataWriter()
{
    delete this->commandQueue;
}

void SerialDataWriter::run()
{
    while(true)
    {
        timeSinceLastPing++;
        if(!commandQueue->empty())
        {
            Command commandToSend = commandQueue->top();
            std::string data = commandToSend.getString().toStdString();
            commandQueue->pop();
            // Probably need to wait for this write to finish
            serialPort->write(data.data(), BYTES_IN_COMMAND);
            emit sentCommand(commandToSend);
        }else{
            if(timeSinceLastPing < TIME_PER_PING_MS || !startPings) continue;

            Command ping = Command(PING_COMMAND);
            // Same with this one
            serialPort->write(PING_COMMAND, BYTES_IN_COMMAND);
            emit sentCommand(ping);
        }
    }
}

void SerialDataWriter::setStartPings()
{
    startPings = true;
}

void SerialDataWriter::addNewCommandToQueue(const Command & command)
{
    commandQueue->push(command);
}

