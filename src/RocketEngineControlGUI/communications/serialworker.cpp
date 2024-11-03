#include "serialworker.h"
#include "mainwindow.h"

SerialWorker::SerialWorker(MainWindow * window, QObject *parent)
    : QThread{parent}, mainWindow(window)
{
    serialPort = new QSerialPort();
    serialPort->moveToThread(this);
    // TODO: Configure Serial Port to match the test stand config
    this->serialPort->setBaudRate(QSerialPort::BaudRate::Baud9600);
    this->serialPort->setParity(QSerialPort::Parity::NoParity);
    this->serialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    this->serialPort->setDataBits(QSerialPort::DataBits::Data8);
    this->serialPort->setStopBits(QSerialPort::StopBits::OneStop);

    this->commandToSend = "";

    connect(
        mainWindow,
        &MainWindow::issueCommand,
        this,
        &SerialWorker::issueCommand,
        Qt::QueuedConnection
    );

    connect(
        mainWindow,
        &MainWindow::startPings,
        this,
        &SerialWorker::setStartPings,
        Qt::QueuedConnection
    );

    connect(
        mainWindow,
        &MainWindow::serialPortChanged,
        this,
        &SerialWorker::onPortNameChange,
        Qt::QueuedConnection
    );

    connect(
        this->serialPort,
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
    while(serialPort->bytesAvailable() >= BYTES_IN_COMMAND)
    {
        // If the current data packet is not yet finished then we
        // want to try and fill the data buffer if possible.
        if(bytesInDataBuffer)
        {
            // The number of bytes to read is either the number available
            // or is the number left to fill the buffer
            int numberOfBytesToRead = std::min(
                (unsigned long long) serialPort->bytesAvailable(),
                (unsigned long long) sizeof(dataBuffer) - bytesInDataBuffer
            );

            // Then attempt a read
            int bytesRead = serialPort->read(
                &dataBuffer[bytesInDataBuffer],
                numberOfBytesToRead
            );

            if(bytesRead == -1)
            {
                qDebug() << "Read Error from Serial Port \n";
                return;
            }

            bytesInDataBuffer += bytesRead;

            if(bytesInDataBuffer == sizeof(dataBuffer))
            {
                this->processSensorData();
            }
        }
        else
        {
            // If there are not bytes in the data buffer then we want to try and read BYTES_IN_COMMAND number of bytes
            // to see if its a command ack or data packet
            bytesInDataBuffer = serialPort->read(dataBuffer, BYTES_IN_COMMAND);

            if(!memcmp(mostRecentlySentCommand.data(), dataBuffer, BYTES_IN_COMMAND)){
                // If this does match the most recent command sent then great emit and continue
                emit commandSuccess(mostRecentlySentCommand);
                bytesInDataBuffer = 0;
                // Well now this command has been sent and received and we can resend pings
                mostRecentlySentCommand = "";
                commandToSend = "";
                commandRetries = 0;
                continue;
            }

            std::string dataPacketHeader = EXPECTED_DATA_HEADER;
            // If the header says that this is data, then we let the code above handle reading the rest of the data
            // and processing it.
            if(!memcmp(dataPacketHeader.data(), dataBuffer, BYTES_IN_COMMAND)) continue;

            // Then lastly if this data is not sensor data or a command ack then its likely corrupted
            // Emit as such
            QSharedPointer<QByteArray> data = QSharedPointer<QByteArray>(new QByteArray(dataBuffer, BYTES_IN_COMMAND));
            bytesInDataBuffer = 0;
            emit corruptedData(data);
        }
    }
}

void SerialWorker::writeOperation()
{
    // If not enough time has elapsed then increment the
    // time since last command and wait
    if(timeSinceLastCommand < COMMAND_WAIT_MS)
    {
        timeSinceLastCommand += WORKER_LOOP_YIELD_MS;
        return;
    }

    // If a command has exceeded the max retries and we have not gotten
    // an acknowledgement then the command has failed.
    if(commandRetries > MAX_COMMAND_RETRIES)
    {
        emit commandFailed(commandToSend);
        commandToSend = "";
        commandRetries = 0;
        return;
    }

    // If there is a command to write
    if(commandToSend != "")
    {
        // Try writing
        int bytesWritten = serialPort->write(commandToSend.data(), BYTES_IN_COMMAND);
        if(bytesWritten < BYTES_IN_COMMAND)
        {
            qDebug() << "Serial Command: " << commandToSend <<
                " Failed to send on attempt: " << commandRetries + 1 <<
                " only " << bytesWritten << "/" << BYTES_IN_COMMAND << "written \n";
        }

        // Then emit that there was an attempt and update variables
        emit commandAttempt(commandToSend);
        commandRetries++;
        timeSinceLastCommand = 0;
        mostRecentlySentCommand = commandToSend;
        return;
    }

    // If there is no command to write then try sending a ping.
    int bytesWritten = serialPort->write(PING_COMMAND, BYTES_IN_COMMAND);
    if(bytesWritten < BYTES_IN_COMMAND)
    {
        qDebug() << "Serial Command: " << PING_COMMAND <<
            " Failed to send on attempt: " << commandRetries + 1 <<
            " only " << bytesWritten << "/" << BYTES_IN_COMMAND << "written \n";
    }

    // Then update variables and emit attempt of ping
    emit commandAttempt(std::string(PING_COMMAND));
    commandRetries++;
    timeSinceLastCommand = 0;
    mostRecentlySentCommand = PING_COMMAND;
}

void SerialWorker::checksum12(void *checksum, const void *data, int n) {
    uint8_t* checksumPtr = (uint8_t *) checksum;
    const uint8_t* dataPtr = (uint8_t *) data;
    memset(checksumPtr, 0, 12);
    for (int i = 0; i < n; ++i) {
        checksumPtr[i % 12] ^= dataPtr[i];
        checksumPtr[(i + 1) % 12] ^= (dataPtr[i] >> 4) | (dataPtr[i] << 4);  // Simple mixing
    }
}

void SerialWorker::processSensorData()
{
    // Compute Checksum
    uint8_t incomingChecksum [CHECKSUM_SIZE];
    this->checksum12(incomingChecksum, dataBuffer, sizeof(SensorData) - CHECKSUM_SIZE);

    // Check if it matches what was given to us
    if(
        memcmp(
            incomingChecksum,
            &dataBuffer[sizeof(SensorData) - CHECKSUM_SIZE],
            CHECKSUM_SIZE
        )
    ) // If the checksums do not match then handle it as if its corrupted
    {
        // Copy into ByteArray and emit as corrupted if Checksum fails
        QSharedPointer<QByteArray> corruptedDataPtr = QSharedPointer<QByteArray>(
            new QByteArray(dataBuffer, sizeof(SensorData))
        );

        emit corruptedData(corruptedDataPtr);
    }
    else
    {
        // Copy to SensorData Struct and emit as Data if valid
        QSharedPointer<SensorData> sensorData = QSharedPointer<SensorData>(new SensorData);
        memcpy(dataBuffer, sensorData.data(), sizeof(SensorData));
        // Convert to metric units
        this->metricUnitConversions(*sensorData);

        emit dataAvailable(sensorData);
    }

    // Indicate that the data buffer has been processed and is now empty.
    bytesInDataBuffer = 0;
}

void SerialWorker::metricUnitConversions(SensorData & data)
{
    throw std::runtime_error("NOT IMPLEMENTED");
}

void SerialWorker::onPortNameChange(const QSerialPortInfo & port)
{
    // Close the port if its open
    if(this->serialPort->isOpen())
        this->serialPort->close();

    // Then try to update and reopen the port
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

    case QSerialPort::UnknownError:
    default:
        emit genericErrorOccurred(error);
        break;

    }
}
