#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtCharts/QtCharts>
#include <QTimer>
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , countdownTimer(nullptr)
{
    ui->setupUi(this);
    this->configureCharts();
    this->handleSerialPortRefresh();
    connect(this->ui->RefreshSerialPorts, &QPushButton::clicked, this, &MainWindow::handleSerialPortRefresh);
    connect(this->ui->AbortButton, &QPushButton::clicked, this, &MainWindow::handleShutdown);
    connect(this->ui->CountdownButton, &QPushButton::clicked, this, &MainWindow::handleCountdown);
}

void MainWindow::keyPressEvent(QKeyEvent* keyEvent)
{
    if(keyEvent->key() == Qt::Key_Backspace)
    {
        this->handleShutdown();
    }
}

MainWindow::~MainWindow()
{
    if (countdownTimer) {
        countdownTimer->stop();
        delete countdownTimer;
    }

    delete ui;
}

void MainWindow::handleSerialPortRefresh()
{
    ui->SerialPortDropdown->clear();
    ui->SerialPortDropdown->addItems(this->getSerialPorts());
}

bool isPaused = false;

void MainWindow::handleCountdown()
{
    if (isPaused) {
        // Resume countdown if it was paused
        resumeCountdown();
        return;
    }

    ui->CountdownButton->setDisabled(true);
    ui->AbortButton->setDisabled(false);
    ui->CountdownButton->setText("Countdown Started");

    // Set the countdown to 5 minutes in milliseconds
    int count = 300000; // 300,000 ms = 5 minutes
    ui->CountdowLabel->setText("5:00:00");

    // Check if countdownTimer already exists; stop and delete it if necessary
    if (countdownTimer) {
        countdownTimer->stop();
        countdownTimer->deleteLater();
    }

    // Create and start the countdown timer
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, [this, count]() mutable {
        count -= 10;

        if (count == 30000) {
            countdownTimer->stop();
            isPaused = true;
            ui->CountdownButton->setEnabled(true);
            ui->CountdownButton->setText("Resume Countdown");
            return;
        }

        // Calculate minutes, seconds, and centiseconds
        int minutes = count / 60000;
        int seconds = (count % 60000) / 1000;
        int centiseconds = (count % 1000) / 10;

        // Update the countdown label in "minutes:seconds:centiseconds" format
        ui->CountdowLabel->setText(QString::number(minutes).rightJustified(2, '0') + ":" +
                                   QString::number(seconds).rightJustified(2, '0') + ":" +
                                   QString::number(centiseconds).rightJustified(2, '0'));

        // If countdown reaches 0, stop the timer
        if (count <= 0) {
            countdownTimer->stop();
            countdownTimer->deleteLater();
            countdownTimer = nullptr;

            ui->CountdownButton->setEnabled(true);
            ui->CountdownButton->setText("Start Countdown");
        }
    });

    countdownTimer->start(10); // 10 ms interval for countdown by centiseconds
}

void MainWindow::resumeCountdown()
{
    isPaused = false;
    ui->CountdownButton->setDisabled(true);
    ui->CountdownButton->setText("Countdown Resumed");
    countdownTimer->start(10); // Resume countdown with a 10 ms interval
}


void MainWindow::handleShutdown()
{
    ui->AbortButton->setDisabled(true);
    ui->CountdownButton->setDisabled(false);

    ui->CountdownButton->setText("Start Countdown");
    ui->AbortButton->setText("Shutdown Started");

    if(countdownTimer){
        countdownTimer->stop();
    }

    ui->AbortButton->setStyleSheet("#AbortButton {\n	background-color: rgb(119, 118, 123); \n color: rgb(255, 255, 255);}");
    // Shutdown Logic goes below here:
}

QStringList MainWindow::getSerialPorts()
{
    QStringList ports;
    QList<QSerialPortInfo> openPorts = QSerialPortInfo::availablePorts();
    for(auto const & port: openPorts){
        if(port.hasVendorIdentifier()) ports.append(port.portName() + ": " + port.manufacturer());
    }
    return ports;
}

void MainWindow::configureCharts()
{
    ui->LoadCellChart->setChartTitle("Load Cell");
    ui->LoadCellChart->setChartType(ChartType::Force);

    ui->FuelInlet1Chart->setChartTitle("Fuel Inlet 1 Temp");
    ui->FuelInlet1Chart->setChartType(ChartType::Temperature);
    ui->FuelInlet2Chart->setChartTitle("Fuel Inlet 2 Temp");
    ui->FuelInlet2Chart->setChartType(ChartType::Temperature);

    ui->OxidizerInlet1Chart->setChartTitle("Oxidizer Inlet 1 Temp");
    ui->OxidizerInlet1Chart->setChartType(ChartType::Temperature);
    ui->OxidizerInlet2Chart->setChartTitle("Oxidizer Inlet 2 Temp");
    ui->OxidizerInlet2Chart->setChartType(ChartType::Temperature);

    ui->OxidizerTankPressureChart->setChartTitle("Oxidizer Tank Pressure");
    ui->OxidizerTankPressureChart->setChartType(ChartType::Pressure);
    ui->OxidizerLinePressureChart->setChartTitle("Oxidizer Line Pressure");
    ui->OxidizerLinePressureChart->setChartType(ChartType::Pressure);
    ui->FuelTankPressureChart->setChartTitle("Fuel Tank Pressure");
    ui->FuelTankPressureChart->setChartType(ChartType::Pressure);
    ui->FuelLinePressureChart->setChartTitle("Fuel Line Pressure");
    ui->FuelLinePressureChart->setChartType(ChartType::Pressure);
    ui->NitrogenTankPressureChart->setChartTitle("Nitrogen Tank Pressure");
    ui->NitrogenTankPressureChart->setChartType(ChartType::Pressure);
    ui->FuelFeedPressureChart->setChartTitle("Nitrogen Line Pressure");
    ui->FuelFeedPressureChart->setChartType(ChartType::Pressure);
}

