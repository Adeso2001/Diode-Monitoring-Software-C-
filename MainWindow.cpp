#include "MainWindow.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    connectSignals();
    runProgram();
}

void MainWindow::setupUi()
{
    setWindowTitle("Diode Measurement");

    auto *mainLayout = new QVBoxLayout(this);

    // Top bar
    auto *topLayout = new QHBoxLayout();

    timeLabel = new QLabel("Time: 0.0 s", this);
    quitButton = new QPushButton("Quit", this);

    topLayout->addWidget(timeLabel);
    topLayout->addStretch();
    topLayout->addWidget(quitButton);

    mainLayout->addLayout(topLayout);

    // Diode table
    auto *grid = new QGridLayout();

    grid->addWidget(new QLabel("Diode", this), 0, 0);
    grid->addWidget(new QLabel("Voltage", this), 0, 1);
    grid->addWidget(new QLabel("Temperature", this), 0, 2);
    grid->addWidget(new QLabel("On", this), 0, 3);
    grid->addWidget(new QLabel("Off", this), 0, 4);
    grid->addWidget(new QLabel("Calibrate", this), 0, 5);

    for (int i = 0; i < 16; ++i)
    {
        int row = i + 1;

        auto *diodeName = new QLabel(QString("D%1").arg(i), this);

        voltageLabels[i] = new QLabel("-- V", this);
        temperatureLabels[i] = new QLabel("-- K", this);

        onButtons[i] = new QPushButton("On", this);
        offButtons[i] = new QPushButton("Off", this);
        calibrateButtons[i] = new QPushButton("Calibrate", this);

        grid->addWidget(diodeName, row, 0);
        grid->addWidget(voltageLabels[i], row, 1);
        grid->addWidget(temperatureLabels[i], row, 2);
        grid->addWidget(onButtons[i], row, 3);
        grid->addWidget(offButtons[i], row, 4);
        grid->addWidget(calibrateButtons[i], row, 5);
    }

    mainLayout->addLayout(grid);

    setLayout(mainLayout);
    resize(700, 500);

    updateTimer = new QTimer(this);

    connect(updateTimer, &QTimer::timeout, this, [this]() {
        
        auto temperatures = diode.getCurrentTemperatures();
        auto voltages = diode.getCurrentVoltages();

        timeLabel->setText(QString("Time: %1 s").arg(diode.getCurrentTime(), 0, 'f', 2));
        
        for (int i = 0; i < 16; ++i)
        {
            voltageLabels[i]->setText(QString("%1 V").arg(voltages[i], 0, 'f', 4));
            temperatureLabels[i]->setText(QString("%1 K").arg(temperatures[i], 0, 'f', 2));
        }
    });

updateTimer->start(500);
}

void MainWindow::connectSignals()
{
    connect(quitButton, &QPushButton::clicked,
            this, &MainWindow::onQuitClicked);

    for (int i = 0; i < 16; ++i)
    {
        connect(onButtons[i], &QPushButton::clicked, this, [this, i]() {
            diodeToggleOn(i);
        });

        connect(offButtons[i], &QPushButton::clicked, this, [this, i]() {
            diodeToggleOff(i);
        });

        connect(calibrateButtons[i], &QPushButton::clicked, this, [this, i]() {
            diodeCalibrate(i);
        });
    }
}

void MainWindow::diodeToggleOn(int diodeNumber)
{
    std::string command = "D ON " + std::to_string(diodeNumber);
    diode.queueMainCommand(command);
}

void MainWindow::diodeToggleOff(int diodeNumber)
{
    std::string command = "D OFF " + std::to_string(diodeNumber);
    diode.queueMainCommand(command);
}

void MainWindow::diodeCalibrate(int diodeNumber)
{
    std::string command = "CALIBRATE " + std::to_string(diodeNumber);
    diode.queueMainCommand(command);
}

void MainWindow::onQuitClicked()
{
    std::string command = "QUIT";
    diode.queueMainCommand(command);
    close();
}

void MainWindow::runProgram()
{
    diode.run();
}