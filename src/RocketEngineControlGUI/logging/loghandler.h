#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <QFile>
#include <QSharedPointer>
#include <QDateTime>
#include <QException>


#include "communications/sensordata.h"

enum class EventType: uint8_t {
    CommandSent = 0,
    AcknowledgementReceived,
    CommandFailed,
    SerialError,
    CorruptedData,
    Debug,
    Info,
    Warning,
    Error
};

class LogHandler
{
public:
    LogHandler(
        QString outputPath,
        QString dataFileName = "default",
        QString eventFileName = "default",
        QString corruptedDataFileName = "default"
    );
    ~LogHandler();

public slots:
    void logData(const QSharedPointer<SensorData> data);
    void logEvent(EventType eventType, const QString & message);
    void logCorruptedData(const QSharedPointer<QByteArray> corruptedData);

protected:
    bool openFile(QFile & fileToOpen);

private:
    QFile dataLog, eventLog, corruptionLog;
    QTextStream dataStream, eventStream, corruptionStream;
};

#endif // LOGHANDLER_H
