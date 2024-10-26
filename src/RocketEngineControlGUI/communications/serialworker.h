#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <cmath>
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

// Forward Declaration of the MainWindow to avoid circular include dependencies
class MainWindow;

class SerialWorker : public QThread
{
    Q_OBJECT
public:
    explicit SerialWorker(MainWindow * window, QObject *parent = nullptr);
    void run() override;

protected:
    void readOperation();
    void writeOperation();
    void checksum12(void * checksum, const void * data, int n);
    void processSensorData();

public slots:
    void onPortNameChange(const QSerialPortInfo & port);
    void issueCommand(const std::string & command);
    void setStartPings(bool value);
    void handleSerialError(QSerialPort::SerialPortError error);

signals:
    void commandAttempt(std::string command);
    void commandFailed(std::string command);
    void commandSuccess(std::string command);
    void dataAvailable(const QSharedPointer<SensorData> data);
    void corruptedData(const QSharedPointer<QByteArray> data);
    void portOpenFailed();
    void portOpenSuccess();

    // Serial Error Signals
    void readErrorOccurred();
    void resourceErrorOccurred();
    void permissionErrorOccurred();
    void genericErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort* serialPort;
    std::string commandToSend;
    std::string mostRecentlySentCommand;
    char dataBuffer[sizeof(SensorData)];
    size_t bytesInDataBuffer;
    bool startPings = false;
    size_t timeSinceLastCommand = COMMAND_WAIT_MS;
    size_t commandRetries = 0;
    MainWindow * mainWindow;
};

#endif // SERIALWORKER_H
