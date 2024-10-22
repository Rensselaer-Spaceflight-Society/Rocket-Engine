#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>

#include "communications/serialdatawriter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void configureCharts();
    QStringList getSerialPorts();

private slots:
    void handleShutdown();
    void handleSerialPortRefresh();
    void handleSerialPortSelection(int portIndex);

signals:
    void issueCommand(const std::string & command);
    void startPings();

protected:
    void keyPressEvent(QKeyEvent* keyEvent) override;

private:
    Ui::MainWindow *ui;
    QSerialPort *commsPort;
    SerialDataWriter *commandSender;

    QList<QSerialPortInfo> availableSerialPorts;
};
#endif // MAINWINDOW_H
