#include "serialworker.h"
#include "mainwindow.h"

SerialWorker::SerialWorker(MainWindow * window, QObject *parent)
    : mainWindow(window),
    serialPort(new QSerialPort(this)),
    dataBuffer(new QByteArray(sizeof(SensorData), 0)),
    commandTimer(new QTimer(this))
{
    // TODO: Configure Serial Port to match the test stand config
    this->serialPort->setBaudRate(QSerialPort::BaudRate::Baud9600);
    this->serialPort->setParity(QSerialPort::Parity::NoParity);
    this->serialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    this->serialPort->setDataBits(QSerialPort::DataBits::Data8);
    this->serialPort->setStopBits(QSerialPort::StopBits::OneStop);

    this->commandToSend = "";

    connect(commandTimer, &QTimer::timeout, this, &SerialWorker::handleTimeout);
    connect(serialPort, &QSerialPort::readyRead, this, &SerialWorker::handleReadReady);
    connect(serialPort, &QSerialPort::errorOccurred, this, &SerialWorker::handleSerialError);

    commandTimer->start(COMMAND_SEND_TIMEOUT_DURRATION_MS);
}

SerialWorker::~SerialWorker()
{
    delete serialPort;
    delete dataBuffer;
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
    // 1. Compute the checksum of the incomming data
    QByteArray incommingDataChecksum(CHECKSUM_SIZE, 0);
    checksum12(incommingDataChecksum.data(), dataBuffer->data(), sizeof(SensorData)-CHECKSUM_SIZE);

    // 2. Compare the computed checksum and the existing one
    const char * receivedChecksumPointer = dataBuffer->data() + sizeof(SensorData)-CHECKSUM_SIZE;
    if(!memcmp(incommingDataChecksum.data(), receivedChecksumPointer, CHECKSUM_SIZE))
    {
        // If the checksums match that means data wasn't modified in transit so we send to be graphed
        SensorData data = {0};
        memcpy(&data, dataBuffer->data(), sizeof(SensorData));
        emit dataAvailable(data);
    }
    else
    {
        // If the checksums don't match then we emit as corrupted data
        QByteArray corruptedDataBuffer = *dataBuffer;
        emit corruptedData(corruptedDataBuffer);
    }
}

void SerialWorker::onPortNameChange(const QSerialPortInfo &port)
{
    if(serialPort->isOpen())
        serialPort->close();

    serialPort -> setPort(port);

    if(serialPort->open(QIODevice::ReadWrite))
    {
        emit portOpenSuccess();
    }
    else
    {
        emit portOpenFailed();
    }
}

void SerialWorker::setStartPings(bool value)
{
    startPings = value;
}

void SerialWorker::issueCommand(const QString &command)
{
    commandToSend = command;
}

void SerialWorker::handleReadReady()
{
    // If there is not already sensor data in the buffer and there is not a full command
    // wait until there is a full command
    if(dataBuffer->size() == 0 && serialPort->bytesAvailable() < BYTES_IN_COMMAND) return;

    // If there is already sensor data then we should try to fill the buffer and process it
    if(dataBuffer->size() > 0)
    {
        dataBuffer->append(serialPort->read(sizeof(SensorData)-dataBuffer->size()));
        if(dataBuffer->size() == sizeof(SensorData))
        {
            processSensorData();
            // After processing clear the buffer to indicate that we can move to the next packet
            dataBuffer->clear();
        }
        // If we cannot fill the buffer, then we return and wait until the read ready signal is sent
        // to continue to fill the buffer
        return;
    }

    // If there is not already sensor data then we read 8 bytes and check its value to see if its a
    // data header, engine signal, or corrupted data
    dataBuffer->append(serialPort->read(BYTES_IN_COMMAND));

    // Then check if its a sensor data header and if it is then just leave it in the buffer and wait for more
    // data

    if(!memcmp(dataBuffer->data(), EXPECTED_DATA_HEADER, BYTES_IN_COMMAND)) return;

    // Check if its any of the signals and emit if it is
    for(const auto & signal: EngineSignals) {
        if(!memcmp(dataBuffer->data(), signal.data(), BYTES_IN_COMMAND))
        {
            commandToSend = "";
            commandRetries = 0;
            emit signalReceived(signal);
            return;
        }
    }

    // If the data is not an engine signal then we have an invalid signal and we can copy the buffer and emit as
    // corrupted data and clear the data buffer

    QByteArray bufferCopy = *dataBuffer;
    emit corruptedData(bufferCopy);
    dataBuffer->clear();

}

void SerialWorker::handleSerialError(QSerialPort::SerialPortError error)
{
    emit serialErrorOccurred(error, serialPort->errorString());
}

void SerialWorker::handleTimeout()
{
    // If the serial port is not open then don't try to write
    if(!(serialPort->isOpen())) return;

    // If we have a command to send then send that command
    if(!(commandToSend.isEmpty()))
    {
        // But if we have exceeded the max command attempts then say this command failed
        if(commandRetries >= MAX_COMMAND_RETRIES)
        {
            emit commandFailed(commandToSend);
        }

        serialPort->write(commandToSend.toUtf8());
        commandRetries++;
        emit commandAttempt(commandToSend);
    }

    // Don't start sending pings until startPings is set to be true
    if(!startPings) return;

    serialPort->write(PING_COMMAND);
    emit commandAttempt(PING_COMMAND);
}

