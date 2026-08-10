#include "MainWindow.h"

#include <filesystem>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>

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
    std::string filePath;

    // bring up file dialog to select calibration file
    QString fileName = QFileDialog::getOpenFileName(this, "Select Calibration File", "",
         "All Files (*.*);;Text Files (*.txt);;CSV Files (*.csv);");

    // if file selected not in folder, copy file to folder
    if (!fileName.isEmpty()) {
    QFileInfo fileInfo(fileName);

    // test if file in calibration directory, if not copy file to directory
    bool isInCalibrationDir =
        fileInfo.dir().canonicalPath() == QDir(calibrationDir).canonicalPath();

    if (!isInCalibrationDir) {
        // Test if file with same name exists in calibration directory, if so, bring up message box to
        // ask if user wants to overwrite file

        QString destFilePath = calibrationDir + "/" + fileInfo.fileName();
        if (QFile::exists(destFilePath)) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Overwrite File",
                                          "A file with the same name already exists in the calibration directory. Do you want to overwrite it?",
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) {
                return; // User chose not to overwrite, exit the function
            }
        }
        // Copy the file to the calibration directory
        if (!QFile::copy(fileName, destFilePath)) {
            QMessageBox::critical(this, "Error", "Failed to copy the file to the calibration directory.");
            return; // Exit the function if copy fails
        }

        filePath = destFilePath.toStdString();

    }
    else
    {
        filePath = fileName.toStdString();
    }

    // set the calibration file name in DiodeProgram
    diode.setCalibrationFilePath(filePath, diodeNumber);
    std::cout<<"filepath is "<<filePath<<std::endl;

    // add command to main command queue to calibrate diode
    std::string command = "CALIBRATE";
    diode.queueMainCommand(command);
    }
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