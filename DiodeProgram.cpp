#include "DiodeProgram.h"

#include <map>

// ============================================================
// Command multimaps
// ============================================================

// Multimap for main thread commands, maps command strings to integers representing which thread 
// should carry out the command.
std::multimap<std::string, int>& mainCommandQueueDirector()
{
    static std::multimap<std::string, int> map;

    /* Maps commands onto integers, which dictaten which thread needs to execute command.
        * 0 = main thread
        * 1 = DAQManager thread
        * 2 = ArduinoNano thread
        * 3 = SerialPort thread
    */
    map = {
        {"QUIT", 0},
        {"START DAQ", 1},
        {"STOP DAQ", 1},
        {"CALIBRATE", 1},
        {"D ON 0", 2},
        {"D OFF 0", 2},
        {"D ON 1", 2},
        {"D OFF 1", 2},
        {"D ON 2", 2},
        {"D OFF 2", 2},
        {"D ON 3", 2},
        {"D OFF 3", 2},
        {"D ON 4", 2},
        {"D OFF 4", 2},
        {"D ON 5", 2},
        {"D OFF 5", 2},
        {"D ON 6", 2},
        {"D OFF 6", 2},
        {"D ON 6", 2},
        {"D OFF 6", 2},
        {"D ON 6", 2},
        {"D OFF 6", 2},
        {"D ON 7", 2},
        {"D OFF 7", 2},
        {"D ON 8", 2},
        {"D OFF 8", 2},
        {"D ON 9", 2},
        {"D OFF 9", 2},
        {"D ON 10", 2},
        {"D OFF 10", 2},
        {"D ON 11", 2},
        {"D OFF 11", 2},
        {"D ON 12", 2},
        {"D OFF 12", 2},
        {"D ON 13", 2},
        {"D OFF 13", 2},
        {"D ON 14", 2},
        {"D OFF 14", 2},
        {"D ON 15", 2},
        {"D OFF 15", 2}
    };

    return map;
}

// enum to make defining main thread commands easier
enum class MainCommands
{
    QUIT = 0
};

// Map for main thread commands, maps command strings to MainCommands enum values representing which command to carry out.
std::map<std::string, MainCommands>& mainCommandMap()
{
    static std::map<std::string, MainCommands> map;

    map = {
        {"QUIT", MainCommands::QUIT}
    };

    return map;
}

// enum to make defining arduino commands easier
enum class ArduinoCommands
{
    DIODE_0_ON = 0,
    DIODE_0_OFF = 1,
    DIODE_1_ON = 2,
    DIODE_1_OFF = 3,
    DIODE_2_ON = 4,
    DIODE_2_OFF = 5,
    DIODE_3_ON = 6,
    DIODE_3_OFF = 7,
    DIODE_4_ON = 8,
    DIODE_4_OFF = 9,
    DIODE_5_ON = 10,
    DIODE_5_OFF = 11,
    DIODE_6_ON = 12,
    DIODE_6_OFF = 13,
    DIODE_7_ON = 14,
    DIODE_7_OFF = 15,
    DIODE_8_ON = 16,
    DIODE_8_OFF = 17,
    DIODE_9_ON = 18,
    DIODE_9_OFF = 19,
    DIODE_10_ON = 20,
    DIODE_10_OFF = 21,
    DIODE_11_ON = 22,
    DIODE_11_OFF = 23,
    DIODE_12_ON = 24,
    DIODE_12_OFF = 25,
    DIODE_13_ON = 26,
    DIODE_13_OFF = 27,
    DIODE_14_ON = 28,
    DIODE_14_OFF = 29,
    DIODE_15_ON = 30,
    DIODE_15_OFF = 31
};

// Map for ArduinoNano thread commands, maps command strings to ArduinoCommands enum values 
// representing which pin to set high or low.
std::map<std::string, ArduinoCommands>& arduinoCommandMap()
{
    static std::map<std::string, ArduinoCommands> map;

    map = {
        {"D ON 0", ArduinoCommands::DIODE_0_ON},
        {"D OFF 0", ArduinoCommands::DIODE_0_OFF},
        {"D ON 1", ArduinoCommands::DIODE_1_ON},
        {"D OFF 1", ArduinoCommands::DIODE_1_OFF},
        {"D ON 2", ArduinoCommands::DIODE_2_ON},
        {"D OFF 2", ArduinoCommands::DIODE_2_OFF},
        {"D ON 3", ArduinoCommands::DIODE_3_ON},
        {"D OFF 3", ArduinoCommands::DIODE_3_OFF},
        {"D ON 4", ArduinoCommands::DIODE_4_ON},
        {"D OFF 4", ArduinoCommands::DIODE_4_OFF},
        {"D ON 5", ArduinoCommands::DIODE_5_ON},
        {"D OFF 5", ArduinoCommands::DIODE_5_OFF},
        {"D ON 6", ArduinoCommands::DIODE_6_ON},
        {"D OFF 6", ArduinoCommands::DIODE_6_OFF},
        {"D ON 7", ArduinoCommands::DIODE_7_ON},
        {"D OFF 7", ArduinoCommands::DIODE_7_OFF},
        {"D ON 8", ArduinoCommands::DIODE_8_ON},
        {"D OFF 8", ArduinoCommands::DIODE_8_OFF},
        {"D ON 9", ArduinoCommands::DIODE_9_ON},
        {"D OFF 9", ArduinoCommands::DIODE_9_OFF},
        {"D ON 10", ArduinoCommands::DIODE_10_ON},
        {"D OFF 10", ArduinoCommands::DIODE_10_OFF},
        {"D ON 11", ArduinoCommands::DIODE_11_ON},
        {"D OFF 11", ArduinoCommands::DIODE_11_OFF},
        {"D ON 12", ArduinoCommands::DIODE_12_ON},
        {"D OFF 12", ArduinoCommands::DIODE_12_OFF},
        {"D ON 13", ArduinoCommands::DIODE_13_ON},
        {"D OFF 13", ArduinoCommands::DIODE_13_OFF},
        {"D ON 14", ArduinoCommands::DIODE_14_ON},
        {"D OFF 14", ArduinoCommands::DIODE_14_OFF},
        {"D ON 15", ArduinoCommands::DIODE_15_ON},
        {"D OFF 15",ArduinoCommands::DIODE_15_OFF}
    };

    return map;
}

// enum to make defining DAQManager commands easier
enum class DAQCommands
{
    START_DAQ = 0,
    STOP_DAQ = 1,
    CALIBRATE = 2
};

// Map for DAQManager thread commands, maps command strings to DAQCommands enum values representing
// which command to carry out.
std::map<std::string, DAQCommands>& daqCommandMap()
{
    static std::map<std::string, DAQCommands> map;

    map = {
        {"START DAQ", DAQCommands::START_DAQ},
        {"STOP DAQ", DAQCommands::STOP_DAQ},
        {"CALIBRATE", DAQCommands::CALIBRATE}
    };

    return map;
}

// ============================================================
// DiodeProgram
// ============================================================


DiodeProgram::DiodeProgram()
{
    // any necessary initialization upon construction can be done here  
}

// default destructor, will clean up resources when DiodeProgram object goes out of scope
DiodeProgram::~DiodeProgram()
{
    if (!shuttingDown)
    {
        shuttingDown = true; // signal shutdown if it isn't already
        shutdownProcedure();
    }

    if (mainThread.joinable()) 
    {
        mainThreadRunning = false; // signal main thread to stop if it isn't already
        mainThread.join();
    }
    
}

// ============================================================
// Private helper functions
// ============================================================

// Function which runs DAQManager loop, to be run in a separate thread. Initializes DAQManager, 
// then continuously takes data from it and carries out commands until the program is quit.
void DiodeProgram::daqManagerLoop()
{
    // initialise DAQManager
    DAQManager daqManager;

    //FOR TESTING, REMOVE LATER
    /*
    for (int channel_iterator = 0; channel_iterator < 16; ++channel_iterator)
    {
        daqManager.calibrate_from_file(channel_iterator, "/home/cryolab/DiodeApplication/Diode-Monitoring-Software-C-/Calibration_Files/Diode_A2_Calibration.330");
    }
    */
    daqManager.start_reading();

    std::cout << "DAQ startup successful" << std::endl;

    // create file name for csv file with current date and time
    char date_string[20];

    auto now = std::time(nullptr);
    auto local_time = std::localtime(&now);

    std::strftime(
        date_string,
        sizeof(date_string),
        "%Y-%m-%d_%H-%M-%S",
        local_time
    );

    csvFileName = "diode_data_" + std::string(date_string) + ".csv";

    // initialise csv file
    std::ofstream csv_file(csvFileName, std::ios::out); // Clear the file
    csv_file.close();
    csv_file.open(csvFileName, std::ios::app); // Reopen for app

    std::cout << "csv file initialized" << std::endl;

    // run loop while atomic true
    while(daqManagerRunning)
    {
        // take buffered data from DAQManager
        auto data = daqManager.take_buffered_data();
        
        // write data to csv file
        if (data.size() != 0)
        {
            for (const auto& row : data)
            {
                for (size_t j = 0; j < row.size(); ++j)
                {
                    csv_file << row[j];
                    if (j < row.size() - 1) csv_file << ",";
                }
                csv_file << std::endl;
            }

            /*
            This prints the data as its produced, commented out as this fills up terminal with junk
            for (const auto& row : data)
            {
                std::cout << "Time: " << row[0] << "s, \n";
                for (size_t j = 0; (j+2) < row.size(); j+=2)
                {
                    std::cout << "CH"<< (j/2) <<": V = "<< row[j+1] << ", T = " << row[j+2] << "\n";
                }
            }
            */
        }

        // update current temperatures and voltages vectors with most recent data, for now just filling in voltage values
        if (data.size() != 0)
        {
            std::lock_guard<std::mutex> lock_current_temperatures(currentTemperaturesMutex);
            std::lock_guard<std::mutex> lock_current_voltages(currentVoltagesMutex);
            std::lock_guard<std::mutex> lock_current_time(currentTimeMutex);
            for (size_t channel_iterator = 0; channel_iterator < currentVoltages.size(); ++channel_iterator)
            {
                currentVoltages[channel_iterator] = data.back()[channel_iterator * 2 + 1];
                currentTemperatures[channel_iterator] = data.back()[channel_iterator * 2 + 2];
                currentTime = data.back()[0];
            }
        }

        /*
        For now this is commented out as it will eventually lead to a crash if program runs long
        enough and isnt actually used anywhere
        // update historical data vector
        if (data.size() != 0)
        {
            std::lock_guard<std::mutex> lock(historicalDataMutex);
            historicalData.insert(
                historicalData.end(),
                std::make_move_iterator(data.begin()),
                std::make_move_iterator(data.end())
            );
        }
        */
        // check command queue and carry out commands
        checkDaqCommands(daqManager);

        // sleep for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // shut down DAQManager
    daqManager.stop_reading();
    std::cout << "DAQManager stopped reading" << std::endl;

    // close csv file
    csv_file.close();
    std::cout << "csv file closed" << std::endl;
}

// Function which runs Arduino Nano loop, to be run in a separate thread. Initializes Arduino Nano,
// then continuously checks for commands to carry out until the program is quit.
void DiodeProgram::arduinoNanoLoop()
{
    // initialise Arduino Nano
    ArduinoNano nanoManager;

    #ifdef _WIN32
        std::string portName = "COM5"; // <-- CHANGE THIS AS NEEDED
    #else
        std::string portName = "/dev/ttyUSB0"; // <-- CHANGE THIS AS NEEDED
    #endif

    if (!nanoManager.connect(portName, 9600)) {
        std::cerr << "Failed to connect to Arduino Nano on port " << portName << std::endl;
        std::cerr << "DAQ reading will continue, pin control will be unavailible" << std::endl;
        return;
    }

    // Reset the Arduino Nano to ensure it's in a known state
    nanoManager.reset();
    std::cout << "Arduino Nano reset" << std::endl;

    // Wait for the Arduino Nano to signal that it's ready
    if (!nanoManager.waitForReady("READY", 300)) {
        std::cerr << "Failed to receive ready signal from Arduino Nano" << std::endl;
        std::cerr << "DAQ reading will continue, pin control will be unavailible" << std::endl;
        return;
    }
    std::cout << "Arduino Nano thread startup successful" << std::endl;

    while(arduinoNanoRunning)
    {
        // check command queue and carry out commands
        checkArduinoCommands(nanoManager);

        // sleep for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // shut down Arduino Nano
    if (nanoManager.isConnected())
    {
        nanoManager.disconnect();
        std::cout << "Arduino Nano disconnected" << std::endl;
    }
}

// Function which runs SerialPort loop, to be run in a separate thread. Initializes SerialPort,
// then continuously checks for commands to carry out and reads incoming data until the program is quit.
void DiodeProgram::serialPortLoop()
{
    SerialPort serialManager;

    #ifdef _WIN32
        const std::string portName = "COM5";   // Windows example: "COM3"
    #else
        const std::string portName = "/dev/ttyUSB1";   // Linux example: "/dev/ttyUSB0"
    #endif
    const int baudRate = 9600;

    if (!serialManager.open(portName)) 
    {
        std::cerr << "Failed to open serial port: "
                  << serialManager.lastError() << std::endl;
        serialPortRunning = false;
        return;
    }

    if (!serialManager.configure(baudRate, 8, 1, 'N')) 
    {
        std::cerr << "Failed to configure serial port: "
                  << serialManager.lastError() << std::endl;
        serialManager.close();
        serialPortRunning = false;
        return;
    }

    // Keep reads short so the thread can stop quickly.
    serialManager.setReadTimeoutMs(50);

    std::string receiveBuffer;

    while (serialPortRunning)
    {
        // Send any queued outgoing messages
        {
            std::lock_guard<std::mutex> lock(serialWriteQueueMutex);

            while (!serialWriteQueue.empty()) 
            {
                std::string message = serialWriteQueue.front();
                serialWriteQueue.pop();

                if (!serialManager.writeLine(message)) 
                {
                    std::cerr << "Serial write failed: "
                              << serialManager.lastError() << std::endl;
                }
            }
        }

        // Read any available incoming serial data
        std::string incoming = serialManager.readAvailableText();

        if (!incoming.empty()) 
        {
            receiveBuffer += incoming;

            // Extract complete newline-terminated commands.
            std::size_t newlinePos = std::string::npos;

            while ((newlinePos = receiveBuffer.find('\n')) != std::string::npos) 
            {
                std::string command = receiveBuffer.substr(0, newlinePos);
                receiveBuffer.erase(0, newlinePos + 1);

                // Remove optional carriage return.
                if (!command.empty() && command.back() == '\r') 
                {
                    command.pop_back();
                }

                if (!command.empty()) 
                {
                    if ((command.substr(0, 2) == "V ") || (command.substr(0, 2) == "T "))
                    {
                        serialTVRequest(command);
                    }
                    else
                    {
                        std::lock_guard<std::mutex> lock(mainCommandQueueMutex);
                        std::cout<<"Pushing command to main queue:"<<command<<"/n";
                        mainCommandQueue.push(command);
                    }
                }
            }
        }

        // Sleep briefly to avoid busy-waiting. Adjust as needed for responsiveness vs CPU usage.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    serialManager.close();
}

// Function which runs main application loop, to be run in a separate thread. Continuously checks 
// for commands to carry out until the program is quit. Acts as an intermediary between the 
// other threads.
void DiodeProgram::mainAppLoop()
{
    // Initialisation?

    while(mainThreadRunning)
    {
        // check command queue and carry out commands
        checkMainCommands();

        // sleep for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!shuttingDown)
    {
        shuttingDown = true; // signal shutdown if it isn't already
        shutdownProcedure();
    }
}

// Initialises all relevant threads and resources for the program. Option left open for error handling, 
// currently just returns true at the end.
bool DiodeProgram::startupProcedure()
{
    // start DAQManager thread
    daqManagerRunning = true;
    daqManagerThread = std::thread(&DiodeProgram::daqManagerLoop, this);
    
    // start Arduino Nano thread
    arduinoNanoRunning = true;
    arduinoNanoThread = std::thread(&DiodeProgram::arduinoNanoLoop, this);
    
    // start Serial Port thread
    serialPortRunning = true;
    serialPortThread = std::thread(&DiodeProgram::serialPortLoop, this);

    return true;
}

/*
Shuts down all threads and resources for the program in a clean way, 
by signalling threads to stop and joining them. Called at the end of 
main thread loop, and in the destructor to ensure resources are cleaned up 
when DiodeProgram object goes out of scope. 
*/
bool DiodeProgram::shutdownProcedure()
{
    // signal threads to stop
    daqManagerRunning = false;
    arduinoNanoRunning = false;
    serialPortRunning = false;

    // join threads to clean up resources
    if (daqManagerThread.joinable()) daqManagerThread.join();
    std::cout << "DAQManager thread joined" << std::endl;
    if (arduinoNanoThread.joinable()) arduinoNanoThread.join();
    std::cout << "Arduino Nano thread joined" << std::endl;
    if (serialPortThread.joinable()) serialPortThread.join();
    std::cout << "Serial Port thread joined" << std::endl;

    return true;
}

/* Helper function to carry out temperature/voltage requests from SerialPort thread.
Takes command as argument, which is expected to be in the format "V 0" or "T 0" for 
voltage and temperature requests. Isolates the channel number and processes request
in thread safe way. Queues result in the serial write queue. */
void DiodeProgram::serialTVRequest(std::string &command)
{
    int dataFlag{0};
    int index;
    std::string response;
    if (command[0] == 'T') {dataFlag = 1;}

    command.erase(0, 2); // Remove the "V " or "T " prefix
    
    // Convert the remaining string to an integer index
    try
    {
        index = std::stoi(command); // Convert the remaining string to an integer index
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error converting command to integer: " << e.what() << '\n';
        std::cerr << "Ignoring malformed command: " << command << std::endl;
        return;
    }

    // Check if index is within bounds
    if (index < 0 || index >= currentVoltages.size())
    {
        std::cerr << "Index out of range in command: " << command << std::endl;
        return;
    }

    // Retrieve the requested value in a thread-safe manner
    if (dataFlag == 1)
    {
        std::lock_guard<std::mutex> lock_current_temperatures(currentTemperaturesMutex);
        response = std::to_string(currentTemperatures[index]);
    }
    else
    {
        std::lock_guard<std::mutex> lock_current_voltages(currentVoltagesMutex);
        response = std::to_string(currentVoltages[index]);
    }

    // Queue the response for sending back over serial
    {
        std::lock_guard<std::mutex> lock(serialWriteQueueMutex);
        serialWriteQueue.push(response);
    }
}

// checks over all commands in the diode command queue, sending them to be carried out
// then removing them in a thread safe manner.
void DiodeProgram::checkDaqCommands(DAQManager &daqManager)
{
   std::lock_guard<std::mutex> lock(daqCommandQueueMutex);
    while (!daqCommandQueue.empty())
    {
        // get command at front of queue and remove it from the queue
        std::string command = daqCommandQueue.front();
        daqCommandQueue.pop();

        carryOutDaqCommand(command, daqManager);
    }
}

// carries out a diode control command, which are specified using DAQCommands enum values and
// daqCommandMap map.
void DiodeProgram::carryOutDaqCommand(std::string &command, DAQManager &daqManager)
{
    int inputIndex;
    std::string inputfilePath;

    std::map<std::string, DAQCommands> commandMap = daqCommandMap();
    auto commandIndex = commandMap.find(command);

    // if command is not found in map, print error and return
    if (commandIndex == commandMap.end())
    {
        std::cout << "Invalid DAQ Thread command: " << command << std::endl;
        return;
    }

    std::cout << "DAQ Thread command executing: " << command << std::endl; 

    // Execute the corresponding command based on the enum value
    switch (commandIndex->second)
    {
        case DAQCommands::START_DAQ:
            if (daqManager.get_read_state())
            {
                std::cout << "DAQ is already reading, ignoring start command" << std::endl;
                return;
            }
            else
            {
                std::cout << "Starting DAQ..." << std::endl;
                daqManager.start_reading();
                break;
            }

        case DAQCommands::STOP_DAQ:
            if (!daqManager.get_read_state())
            {
                std::cout << "DAQ is already stopped, ignoring stop command" << std::endl;
                return;
            }
            else
            {
                std::cout << "Stopping DAQ..." << std::endl;
                daqManager.stop_reading();
                break;
            }
            
        case DAQCommands::CALIBRATE:

            std::cout << "Retreiving calibration info" << std::endl;

            {
                std::lock_guard<std::mutex> lock(calibrationFileMutex);
                inputIndex = calibrationIndex;
                inputfilePath = calibrationFilePath;
            }

            std::cout << "Calibrating channel " << inputIndex << " with file: " << inputfilePath << std::endl;
            daqManager.calibrate_from_file(inputIndex, inputfilePath);
            break;
        default:
            std::cout << "Unknown DAQ command: " << command << std::endl;
    }
}

// checks over all commands in the arduino command queue, sending them to be carried out
// then removing them in a thread safe manner.
void DiodeProgram::checkArduinoCommands(ArduinoNano &arduinoNano)
{
   std::lock_guard<std::mutex> lock(arduinoCommandQueueMutex);
    while (!arduinoCommandQueue.empty())
    {
        // get command at front of queue and remove it from the queue
        std::string command = arduinoCommandQueue.front();
        arduinoCommandQueue.pop();

        carryOutArduinoCommand(command, arduinoNano);
    }
}

// carries out an Arduino control command, which are specified using ArduinoCommands enum values and
// arduinoCommandMap map.
void DiodeProgram::carryOutArduinoCommand(std::string &command, ArduinoNano &arduinoNano)
{
    std::map<std::string, ArduinoCommands> commandMap = arduinoCommandMap();
    auto commandIndex = commandMap.find(command);

    // if command is not found in map, print error and return
    if (commandIndex == commandMap.end())
    {
        std::cout << "Invalid Arduino Thread command: " << command << std::endl;
        return;
    }

    std::cout << "Arduino Thread command executing: " << command << std::endl;

    // Execute the corresponding command based on the enum value
    switch (commandIndex->second)
    {
        case ArduinoCommands::DIODE_0_ON:
        /* TODO: NOTE SINCE ARDUINO PINS 0 AND 1 NON-FUNCTIONAL, CURRENT VERSION HAS PINS 1 AND 2 
        SHORTED WITH 3, WHICH IS FUNCTIONAL  */
            arduinoNano.setPinHigh(2); 
            break;
        case ArduinoCommands::DIODE_0_OFF:
            arduinoNano.setPinLow(2); 
            break;
        case ArduinoCommands::DIODE_1_ON:
            arduinoNano.setPinHigh(2); 
            break;
        case ArduinoCommands::DIODE_1_OFF:
            arduinoNano.setPinLow(2); 
            break;
        case ArduinoCommands::DIODE_2_ON:
            arduinoNano.setPinHigh(2); 
            break;
        case ArduinoCommands::DIODE_2_OFF:
            arduinoNano.setPinLow(2); 
            break;
        case ArduinoCommands::DIODE_3_ON:
            arduinoNano.setPinHigh(3);
            break;
        case ArduinoCommands::DIODE_3_OFF:
            arduinoNano.setPinLow(3); 
            break;
        case ArduinoCommands::DIODE_4_ON:
            arduinoNano.setPinHigh(4);
            break;
        case ArduinoCommands::DIODE_4_OFF:
            arduinoNano.setPinLow(4);
            break;
        case ArduinoCommands::DIODE_5_ON:
            arduinoNano.setPinHigh(5);
            break;
        case ArduinoCommands::DIODE_5_OFF:
            arduinoNano.setPinLow(5);
            break;
        case ArduinoCommands::DIODE_6_ON:
            arduinoNano.setPinHigh(6);
            break;
        case ArduinoCommands::DIODE_6_OFF:
            arduinoNano.setPinLow(6);
            break;
        case ArduinoCommands::DIODE_7_ON:
            arduinoNano.setPinHigh(7);
            break;
        case ArduinoCommands::DIODE_7_OFF:
            arduinoNano.setPinLow(7);
            break;
        case ArduinoCommands::DIODE_8_ON:
            arduinoNano.setPinHigh(8);
            break;
        case ArduinoCommands::DIODE_8_OFF:
            arduinoNano.setPinLow(8);
            break;
        case ArduinoCommands::DIODE_9_ON:
            arduinoNano.setPinHigh(9);
            break;
        case ArduinoCommands::DIODE_9_OFF:
            arduinoNano.setPinLow(9);
            break;
        case ArduinoCommands::DIODE_10_ON:
            arduinoNano.setPinHigh(10);
            break;
        case ArduinoCommands::DIODE_10_OFF:
            arduinoNano.setPinLow(10);
            break;
        case ArduinoCommands::DIODE_11_ON:
            arduinoNano.setPinHigh(11);
            break;
        case ArduinoCommands::DIODE_11_OFF:
            arduinoNano.setPinLow(11);
            break;
        case ArduinoCommands::DIODE_12_ON:
            arduinoNano.setPinHigh(12);
            break;
        case ArduinoCommands::DIODE_12_OFF:
            arduinoNano.setPinLow(12);
            break;
        case ArduinoCommands::DIODE_13_ON:
            arduinoNano.setPinHigh(13);
            break;
        case ArduinoCommands::DIODE_13_OFF:
            arduinoNano.setPinLow(13);
            break;
        case ArduinoCommands::DIODE_14_ON:
            arduinoNano.setPinHigh(14);
            break;
        case ArduinoCommands::DIODE_14_OFF:
            arduinoNano.setPinLow(14);
            break;
        case ArduinoCommands::DIODE_15_ON:
            arduinoNano.setPinHigh(15);
            break;
        case ArduinoCommands::DIODE_15_OFF:
            arduinoNano.setPinLow(15);
            break;
        default:
            std::cout << "Unknown Arduino command: " << command << std::endl;
    }
}

// checks over all commands in the main command queue, sending them to be carried out or to the relevant 
// command queue then removing them in a thread safe manner.
void DiodeProgram::checkMainCommands()
{
    std::string command;

    while (true)
    {
        // get command at front of queue and remove it from the queue
        {
            std::lock_guard<std::mutex> lock(mainCommandQueueMutex);

            if (mainCommandQueue.empty()) {break;}
            
            command = mainCommandQueue.front();
            mainCommandQueue.pop();
        }

        auto executionLocation = mainCommandQueueDirector().find(command)->second;
        
        // execute command or place into relevant command queue
        if (executionLocation == 0) // main thread command
        {
            carryOutMainCommand(command);
        }
        else if (executionLocation == 1) // Daq command
        {
            std::lock_guard<std::mutex> lock(daqCommandQueueMutex);
            daqCommandQueue.push(command);
        }
        else if (executionLocation == 2) // arduino command
        {
            std::lock_guard<std::mutex> lock(arduinoCommandQueueMutex);
            arduinoCommandQueue.push(command);
        }
        else
        {
            std::cout << "Command unable to be directed to valid execution thread: " << command << std::endl;
        }
    }
}

// carries out a main thread command, which are specified using MainCommands enum values and
// mainCommandMap map.
void DiodeProgram::carryOutMainCommand(std::string &command)
{
    std::map<std::string, MainCommands> commandMap = mainCommandMap();
    auto commandIndex = commandMap.find(command);

    // if command is not found in map, print error and return
    if (commandIndex == commandMap.end())
    {
        std::cout << "Invalid Main Thread command: " << command << std::endl;
        return;
    }

    std::cout << "Main Thread command executing: " << command << std::endl;

    // Execute the corresponding command based on the enum value
    switch (commandIndex->second)
    {
        case MainCommands::QUIT:
            quitApp();
            break;
        default:
            std::cout << "Unknown Main Thread command: " << command << std::endl;
    }
}

// ============================================================
// Public member functions
// ============================================================

void DiodeProgram::run()
{
    if (mainThreadRunning)
    {
        std::cerr << "Main thread already running, cannot run again" << std::endl;
        return;
    }
    // run startup procedure, if it returns false, print error and exit
    if (startupProcedure() == false)
    {
        std::cerr << "Error during startup procedure. Exiting." << std::endl;
        return;
    }

    // start main app loop
    mainThreadRunning = true;
    mainThread = std::thread(&DiodeProgram::mainAppLoop, this);
}

// stops main thread loop, which will trigger shutdown procedure and clean up of all threads 
// and resources.
void DiodeProgram::quitApp()
{
    mainThreadRunning = false;
}

// queues a command to be carried out in the main thread, takes command as argument
void DiodeProgram::queueMainCommand(std::string &command)
{
    std::lock_guard<std::mutex> lock(mainCommandQueueMutex);
    mainCommandQueue.push(command);
}
 
// Returns the current temperatures in a thread safe way
std::vector<double> DiodeProgram::getCurrentTemperatures()
{
    std::lock_guard<std::mutex> lock(currentTemperaturesMutex);
    return currentTemperatures;
}

// Returns the current voltages in a thread safe way
std::vector<double> DiodeProgram::getCurrentVoltages()
{
    std::lock_guard<std::mutex> lock(currentVoltagesMutex);
    return currentVoltages;
}

/*
Commented out for now as it actually isn't used anywhere and will eventually 
lead to a crash if program runs long enough
// Returns the historical data in a thread safe way
std::vector<std::vector<double>> DiodeProgram::getHistoricalData()
{
    std::lock_guard<std::mutex> lock(historicalDataMutex);
    return historicalData;
}
*/

// Returns the current time in a thread safe way
double DiodeProgram::getCurrentTime()
{
    std::lock_guard<std::mutex> lock(currentTimeMutex);
    return currentTime;
}

void DiodeProgram::setCalibrationFilePath(const std::string& filePath, const int& index)
{
    std::lock_guard<std::mutex> lock(calibrationFileMutex);
    calibrationFilePath = filePath;
    calibrationIndex = index;
}