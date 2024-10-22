#include "serialdatawriter.h"

/*
 It shouldn't just send one command, it should send up to 3 commands, 100ms apart
 and it should emit a signal if a command was sent 3 times and never recieved an
 acknolwedgement, in that 60 ms. If it recieves an acknowledgement then it should
 pop the command from the queue. If there are no commands in the queue then it should
send a ping, but only after comms have been established.
*/
void SerialDataWriter::run()
{
    while(true){
        this->msleep(1);
        // If there has not yet been the timeout then wait
        if(timeSinceLastCommand < COMMAND_WAIT_MS) continue;

        // If we have exceeded the max number then emit commandFailed
        if(commandRetries >  MAX_COMMAND_RETRIES)
            commandToSend != "" ?
                emit commandFailed(commandToSend) :
                emit commandFailed(PING_COMMAND);

        // If there is a command to send then attempt to send this command
        if(commandToSend != "")
        {
            this->safeDataWrite(commandToSend.data());
        }
        else // If there is no command to send then just send a ping
        {
            this->safeDataWrite(commandToSend.data());
        }
    }
}

void SerialDataWriter::safeDataWrite(char * data)
{
    if(!(this->serialPort->isWritable())) {
        qDebug() << "Serial Port: " << this->serialPort->portName() << " is not writeable. \n";
        emit commandFailed(commandToSend);
        return;
    }

    qint64 bytesWritten = this->serialPort->write(data);
    this->serialPort->flush();

    if(bytesWritten < BYTES_IN_COMMAND){
        qDebug() << "Failed to Send Comand: " << commandToSend << "\n";
        emit commandFailed(commandToSend);
        return;
    }

    commandRetries++;
    emit sentCommand(commandToSend);
}

void SerialDataWriter::setStartPings()
{
    startPings = true;
}

void SerialDataWriter::issueCommand(const std::string & command)
{
    this->commandToSend = command;
    this->commandRetries = 0;
    this->timeSinceLastCommand = COMMAND_WAIT_MS;
}


void SerialDataWriter::acknowledgementRecieved(const std::string & command)
{
    commandRetries = 0;
    commandToSend = "";
}

