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

    connect(
        this->serialPort.data(),
        &QSerialPort::errorOccurred,
        this,
        &SerialWorker::handleSerialError
    );
}

/**
 * @brief Is the main method for the serial worker thread.
 *
 * Loops until told to stop executing. On each loop yield for like
 * 10ms, then we want to check if there is data available in the serial
 * port and handle reading if needed.
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
    while(true)
    {
        // Yield to reduce tight loops
        this->msleep(WORKER_LOOP_YIELD_MS);

        // Handle the read operations
        this->readOperation();

        // Handle any write operations
        this->writeOperation();
    }
}

/**
 * If there is then we want to pull the BYTES_IN_COMMAND number of
 * bytes, and check if those 8 bytes equal the command that was most
 * recently sent or if they equal a ping and if they are emit a signal
 * to handle them as such. If they are not a command acknowledgement
 * or ping ack, then it is likely data, and we can emit as such.
 * We should continue reading until the buffer is < BYTES_IN_COMMAND
*/

void SerialWorker::readOperation()
{
    while(serialPort->bytesAvailable() >= BYTES_IN_COMMAND){

        if(bytesInDataBuffer == 0)
        {
            // If there is not data in the data buffer then we should read
            // data from the serial port and check if its a command ack.
            serialPort->read(dataBuffer, BYTES_IN_COMMAND);

            // If the first BYTES_IN_COMMAND bytes in the data buffer match
            // the most recently sent command then this is an acknowledgement
            if(memcmp(dataBuffer, mostRecentlySentCommand.c_str(), BYTES_IN_COMMAND) == 0){
                emit commandSuccess(commandToSend);
                continue;
            }else{
                // If they do not match the command then it is likely data, in which case,
            }
        }
        else
        {
            // If there is data in the buffer already then we need to
            // read the rest of the sensor data if it is in the serial port's buffer
            if(serialPort->bytesAvailable() >= sizeof(SensorData) - bytesInDataBuffer)
            {

            }
        }


    }
}

void SerialWorker::writeOperation()
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
-------------------------------------------------------------------------------
Read Error              | readErrorOccurred        | Log and retry or notify user
Resource Error          | resourceErrorOccurred    | Close port, attempt reconnection
Permission Error        | permissionErrorOccurred  | Notify user, elevate permissions
*/
void SerialWorker::handleSerialError(QSerialPort::SerialPortError error)
{
    switch (error) {
    case QSerialPort::NoError:
        break;

    case QSerialPort::ReadError:
        emit readErrorOccurred();
        break;

    case QSerialPort::ResourceError:
        emit resourceErrorOccurred();
        break;

    case QSerialPort::PermissionError:
        emit permissionErrorOccurred();
        break;

    default:
        qDebug() << "Unsupported Serial Port Error Occurred, Error Code: " << error ;
        break;
    }
}
