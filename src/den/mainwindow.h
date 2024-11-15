#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>

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
    void handleCountdown();
    void resumeCountdown();

signals:
    void ignitionStart();
    void inertFlush();
    void presFuel();

protected:
    void keyPressEvent(QKeyEvent* keyEvent) override;

private:
    Ui::MainWindow *ui;
    QTimer *countdownTimer; // Countdown timer

};
#endif // MAINWINDOW_H
