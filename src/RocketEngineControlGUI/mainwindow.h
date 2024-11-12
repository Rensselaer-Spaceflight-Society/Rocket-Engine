#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>

#include "communications/serialworker.h"
#include "logging/loghandler.h"
#include "components/alertdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum class EngineStates : int8_t {
    CONNECTION_FAILURE = -1,
    NO_CONNECTION,
    CONNECTION_ESTABLISHED,
    COUNTDOWN_STARTED,
    AUTO_HOLD,
    PRESTART_NITROGEN_FLUSH,
    PRESSUREIZED_FUEL,
    IGNITION,
    PENDING_SHUTDOWN,
    SHUTDOWN,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();   

private slots:
    void handleShutdown();
    void handleSerialPortRefresh();
    void handleSerialPortSelection(int portIndex);

    void handleCommandAttempt(std::string command);
    void handleCommandFailed(std::string command);
    void handleCommandSuccess(std::string command);
    void handleDataAvailable(const QSharedPointer<SensorData> data);
    void handleCorruptedData(const QSharedPointer<QByteArray> data);
    void handlePortOpenFailed();
    void handlePortOpenSuccess();

    // Serial Error Handlers
    void handleReadErrorOccurred();
    void handleResourceErrorOccurred();
    void handlePermissionErrorOccurred();
    void handleGenericErrorOccurred(QSerialPort::SerialPortError error);

signals:
    void issueCommand(const std::string & command);
    void startPings(bool value);
    void serialPortChanged(QSerialPortInfo port);

protected:
    void configureCharts();
    QStringList getSerialPorts();
    void keyPressEvent(QKeyEvent* keyEvent) override;
    void setupConnections();
    void updateUIWithSensorData(const SensorData & data);

private:
    Ui::MainWindow *ui;
    SerialWorker * commsCenter;
    AlertDialog * userAlert;
    double dataPacketCount = 0;
    LogHandler logger;
    EngineStates currentState = EngineStates::NO_CONNECTION;

    QList<QSerialPortInfo> availableSerialPorts;
};
#endif // MAINWINDOW_H
