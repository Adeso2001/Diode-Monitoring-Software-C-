#ifndef DIODE_PROGRAM_H
#define DIODE_PROGRAM_H

#include "DAQManager.h"
#include "Arduino Nano Manager/ArduinoNano.h"
#include "Arduino Nano Manager/SerialPort.h"

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <queue>

// ============================================================
// DiodeProgram
//
// This class encapsulates the logic required to run the diode
// program. It makes use of the various helper classes to
// measure temperature, control the Arduino pins, and manage the 
// serial connections
// ============================================================

class DiodeProgram
{
    private:
        // data members for managing DAQ and Arduino Nano
        std::vector<double> currentTemperatures{std::vector<double>(16, 0.0)};
        std::vector<double> currentVoltages{std::vector<double>(16, 0.0)};
        std::vector<std::vector<double>> historicalData;
        std::string csvFileName{"diode_data.csv"};
        
        std::thread daqManagerThread; // Thread for DAQManager loop
        std::thread arduinoNanoThread; // Thread for Arduino Nano loop
        std::thread serialPortThread; // Thread for SerialPort loop
        std::thread mainThread; // Thread for main loop

        std::atomic<bool> daqManagerRunning{false}; // Atomic boolean to control the running state of DAQManager thread
        std::atomic<bool> arduinoNanoRunning{false}; // Atomic boolean to control the running state of Arduino Nano thread
        std::atomic<bool> serialPortRunning{false}; // Atomic boolean to control the running state of SerialPort thread
        std::atomic<bool> mainThreadRunning{false}; // Atomic boolean to control the running state of SerialPort thread

        std::queue<std::string> mainCommandQueue; // Command queue for main thread to carry out commands from SerialPort thread
        std::queue<std::string> serialWriteQueue; // Command queue for SerialPort thread to carry out commands from main thread or Arduino Nano thread
        std::queue<std::string> arduinoCommandQueue; // Command queue for Arduino Nano thread to carry out commands from SerialPort thread
        std::queue<std::string> daqCommandQueue; // Command queue for daq control commands

        std::mutex mainCommandQueueMutex; // Mutex for main command queue
        std::mutex serialWriteQueueMutex; // Mutex for serial write queue
        std::mutex arduinoCommandQueueMutex; // Mutex for Arduino Nano command queue
        std::mutex daqCommandQueueMutex; // Mutex for daq command queue
        std::mutex currentTemperaturesMutex; // Mutex for currentTemperatures
        std::mutex currentVoltagesMutex; // Mutex for currentVoltages
        std::mutex historicalDataMutex; // Mutex for historicalData
        std::mutex csvFileMutex; // Mutex for csv file

        // Helper functions for threads to run their loops
        void daqManagerLoop(); // Loop for thread to read from DAQManager
        void arduinoNanoLoop(); // Loop for thread to control Arduino Nano pins
        void serialPortLoop(); // Loop for thread to read from SerialPort and carry out commands
        void mainAppLoop(); // Loop for thread to read from SerialPort and carry out commands

        bool startupProcedure(); // Procedure to run at the start of the program to set up DAQManager and Arduino Nano
        bool shutdownProcedure(); // Procedure to run at the end of the program to cleanly shut down DAQManager and Arduino Nano

        void serialTVRequest(std::string &command); // Helper function to carry out temperature/voltage requests from SerialPort thread, takes command as argument
        
        void checkDaqCommands(DAQManager &daqManager); // Helper function to carry out daq control requests from SerialPort thread, takes command as argument
        void carryOutDaqCommand(std::string &command, DAQManager &daqManager); // Helper function to carry out a daq control command, takes command as argument

        void checkArduinoCommands(ArduinoNano &arduinoNano); // Helper function to carry out commands from SerialPort thread to Arduino Nano thread, takes command as argument
        void carryOutArduinoCommand(std::string &command, ArduinoNano &arduinoNano); // Helper function to carry out a command from SerialPort thread to Arduino Nano thread, takes command as argument

        void checkMainCommands(); // Helper function to carry out commands from SerialPort thread to main thread, takes command as argument
        void carryOutMainCommand(std::string &command); // Helper function to carry out a command from SerialPort thread to main thread, takes command as argument

    public:

        DiodeProgram(); // Constructor
        ~DiodeProgram(); // Destructor

        // public member functions
        void run(); // Runs the loop for the app.
        void quitApp(); // Quits the app by stopping all threads and running shutdown procedure
};











#endif