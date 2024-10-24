#ifndef SERIALDATAWRITER_H
#define SERIALDATAWRITER_H

#include <QSerialPort>
#include <QThread>
#include <QDebug>

#define COMMAND_WAIT_MS 200
#define BYTES_IN_COMMAND 8
#define MAX_COMMAND_RETRIES 3
#define CONTROL_ACTIVE_COMMAND "CtrlActi"
#define PING_COMMAND "PingPong"
#define SHUTDOWN_COMMAND "ShutDown"

class SerialDataWriter : public QThread
{
    Q_OBJECT

public:
    explicit SerialDataWriter(QSerialPort* tx, QObject *parent = nullptr):
        serialPort(tx) {};
    ~SerialDataWriter() {};
    void run() override;
    void safeDataWrite(char* data);

public slots:
    void issueCommand(const std::string & Command);
    void acknowledgementRecieved(const std::string & command);
    void setStartPings();

signals:
    void sentCommand(const std::string & command);
    void commandFailed(const std::string & command);

private:
    QSerialPort * serialPort;
    std::string commandToSend;
    bool startPings = false;
    size_t timeSinceLastCommand = 100;
    size_t commandRetries = 0;
};

#endif // SERIALDATAWRITER_H
