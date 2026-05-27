cmake_minimum_required(VERSION 3.16)

project(DiodeMeasurementApp VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(DiodeMeasurementApp
    main.cpp
    MainWindow.h
    MainWindow.cpp

    DiodeProgram.h
    DiodeProgram.cpp

    DAQManager.h
    DAQManager.cpp

    "Arduino Nano Manager/ArduinoNano.h"
    "Arduino Nano Manager/ArduinoNano.cpp"
    "Arduino Nano Manager/SerialPort.h"
    "Arduino Nano Manager/SerialPort.cpp"

    Calibration_Class.h
    Calibration_Class.cpp

    C_Spline_Class.h
    C_Spline_Class.cpp

    mcc128.h
    mcc128.c

    gpio.h
    gpio.c

    util.h
    util.c

    nist.h
    nist.c

    cJSON.h
    cJSON.c
)

target_link_libraries(DiodeMeasurementApp
    PRIVATE
        Qt6::Widgets
        gpiod
)