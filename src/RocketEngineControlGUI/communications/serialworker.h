#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <string>

#include <QThread>
#include <QSerialPort>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QDebug>

#include "sensordata.h"

#define COMMAND_WAIT_MS 200
#define WORKER_LOOP_YIELD_MS 10
#define BYTES_IN_COMMAND 8
#define MAX_COMMAND_RETRIES 3

// IMPORTANT: ALL COMMANDS MUST BE BYTES_IN_COMMAND NUMBER OF CHARS
// OTHERWISE WE COULD HAVE FUN BUFFER OVERFLOWS
#define CONTROL_ACTIVE_COMMAND "CtrlActi"
#define PING_COMMAND "PingPong"
#define SHUTDOWN_COMMAND "ShutDown"

class SerialWorker : public QThread
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = nullptr);
    void run() override;

protected:
    void readOperation();
    void writeOperation();

public slots:
    void onPortNameChange(const QSerialPortInfo & port);
    void issueCommand(const std::string & command);
    void setStartPings(bool value);
    void handleSerialError(QSerialPort::SerialPortError error);

signals:
    void commandAttempt(const std::string & command);
    void commandFailed(const std::string & command);
    void commandSuccess(const std::string & command);
    void dataAvailable(const QSharedPointer<SensorData> data);
    void portOpenFailed();
    void portOpenSuccess();

    // Serial Error Signals
    void readErrorOccurred();
    void resourceErrorOccurred();
    void permissionErrorOccurred();

private:
    QScopedPointer<QSerialPort> serialPort;
    std::string commandToSend;
    std::string mostRecentlySentCommand;
    char dataBuffer[sizeof(SensorData)];
    size_t bytesInDataBuffer;
    bool startPings = false;
    size_t timeSinceLastCommand = COMMAND_WAIT_MS;
    size_t commandRetries = 0;
};

#endif // SERIALWORKER_H
