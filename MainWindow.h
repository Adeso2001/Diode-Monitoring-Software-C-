#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QWidget>
#include <array>

#include "DiodeProgram.h"

class QLabel;
class QPushButton;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    DiodeProgram diode;

    QLabel *timeLabel;
    QPushButton *quitButton;

    std::array<QLabel*, 16> voltageLabels;
    std::array<QLabel*, 16> temperatureLabels;

    std::array<QPushButton*, 16> onButtons;
    std::array<QPushButton*, 16> offButtons;
    std::array<QPushButton*, 16> calibrateButtons;

    QString calibrationDir =
    "/home/cryolab/DiodeApplication/Diode-Monitoring-Software-C-/Calibration_Files";

    QTimer *updateTimer;

    void setupUi();
    void connectSignals();
    void runProgram();

    void diodeToggleOn(int diodeNumber);
    void diodeToggleOff(int diodeNumber);
    void diodeCalibrate(int diodeNumber);

    void onQuitClicked();
};

#endif