#include "serialworker.h"

SerialWorker::SerialWorker(QObject *parent)
    : QThread{parent}, serialPort(new QSerialPort())
{
    // TODO: Configure Serial Port to match the test stand config
    this->serialPort->setBaudRate(QSerialPort::BaudRate::Baud9600);
    this->serialPort->setParity(QSerialPort::Parity::NoParity);
    this->serialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    this->serialPort->setDataBits(QSerialPort::DataBits::Data8);
    this->serialPort->setStopBits(QSerialPort::StopBits::OneStop);

    this->commandToSend = "";
}

/**
 * @brief Is the main method for the serial worker thread.
 *
 * Loops until told to stop executing. On each loop yield for like
 * 10ms, then we want to check if there is data available in the serial
 * port, if there is then we want to pull the BYTES_IN_COMMAND number of
 * bytes, and check if those 8 bytes equal the command that was most
 * recently sent or if they equal a ping and if they are emit a signal
 * to handle them as such. If they are not a command acknowledgement
 * or ping ack, then it is likely data, and we can emit as such.
 * We should continue reading until the buffer is < BYTES_IN_COMMAND
 *
 * If we get a parity error, then we will read the whole buffer,
 * emit a parity error signal so that the potentially incorrect data
 * can be analyzed later but it won't be included in data logs or
 * as a possible command ack.
 *
 * After reading, we want to do our write operations, we check if
 * there is a command to send and it is time to send the command if
 * both are true, then we should send the command. If there is no
 * command and its time to send a command and we have said to start
 * pings then we will issue a ping command to make sure we still
 * have a solid 2-way connection.
 */
void SerialWorker::run()
{

}

void SerialWorker::onPortNameChange(const QSerialPortInfo & port)
{
    if(this->serialPort->isOpen())
        this->serialPort->close();

    serialPort->setPort(port);
    if(serialPort->open(QIODevice::ReadWrite))
    {
        emit portOpenSuccess();
    }
    else
    {
        emit portOpenFailed();
    }
}

void SerialWorker::issueCommand(const std::string & command)
{
    commandToSend = command;
    commandRetries = 0;
    timeSinceLastCommand = COMMAND_WAIT_MS;
}

void SerialWorker::setStartPings(bool value)
{
    this->startPings = value;
}

/*
Error Type              | Signal to Emit           | Action Plan
----------------------------------------------------------
Parity Error            | parityErrorOccurred      | Log data for review and request retransmission
Framing Error           | framingErrorOccurred     | Log and retry
Overrun Error           | overrunErrorOccurred     | Log, adjust buffer size, or reduce rate
Buffer Overflow Error	| bufferOverflowOccurred   | Log and increase buffer size
Break Condition Error	| breakConditionOccurred   | Log, check physical connection
Read Error              | readErrorOccurred        | Log and retry or notify user
Write Error             | writeErrorOccurred       | Log and retry or notify user
Resource Error          | resourceErrorOccurred    | Close port, attempt reconnection
Timeout Error           | timeoutErrorOccurred     | Log, retry, or fail-safe
Permission Error        | permissionErrorOccurred  | Notify user, elevate permissions
*/
void SerialWorker::handleSerialError(QSerialPort::SerialPortError error)
{

}
