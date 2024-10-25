#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <string>

#include <QThread>
#include <QSerialPort>
#include <QSharedPointer>
#include <QScopedPointer>

#include "sensordata.h"

#define COMMAND_WAIT_MS 200
#define BYTES_IN_COMMAND 8
#define MAX_COMMAND_RETRIES 3
#define CONTROL_ACTIVE_COMMAND "CtrlActi"
#define PING_COMMAND "PingPong"
#define SHUTDOWN_COMMAND "ShutDown"

class SerialWorker : public QThread
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = nullptr);
    void run() override;

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

private:
    QScopedPointer<QSerialPort> serialPort;
    std::string commandToSend;
    bool startPings = false;
    size_t timeSinceLastCommand = COMMAND_WAIT_MS;
    size_t commandRetries = 0;
};

#endif // SERIALWORKER_H
