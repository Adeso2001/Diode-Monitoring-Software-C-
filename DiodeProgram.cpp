#include "DiodeProgram.h"

#include <map>

// ============================================================
// Command multimaps
// ============================================================

// Multimap for main thread commands, maps command strings to integers representing which thread 
// should carry out the command.
std::multimap<std::string, int>& mainCommandMap()
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
        {"D OFF 15", 2},
        {"D ON 16", 2},
        {"D OFF 16", 2}
    };

    return map;
}

// enum to make defining arduino commands easier
enum class ArduinoCommands
{
    DIODE_1_ON = 0,
    DIODE_1_OFF = 1,
    DIODE_2_ON = 2,
    DIODE_2_OFF = 3,
    DIODE_3_ON = 4,
    DIODE_3_OFF = 5,
    DIODE_4_ON = 6,
    DIODE_4_OFF = 7,
    DIODE_5_ON = 8,
    DIODE_5_OFF = 9,
    DIODE_6_ON = 10,
    DIODE_6_OFF = 11,
    DIODE_7_ON = 12,
    DIODE_7_OFF = 13,
    DIODE_8_ON = 14,
    DIODE_8_OFF = 15,
    DIODE_9_ON = 16,
    DIODE_9_OFF = 17,
    DIODE_10_ON = 18,
    DIODE_10_OFF = 19,
    DIODE_11_ON = 20,
    DIODE_11_OFF = 21,
    DIODE_12_ON = 22,
    DIODE_12_OFF = 23,
    DIODE_13_ON = 24,
    DIODE_13_OFF = 25,
    DIODE_14_ON = 26,
    DIODE_14_OFF = 27,
    DIODE_15_ON = 28,
    DIODE_15_OFF = 29,
    DIODE_16_ON = 30,
    DIODE_16_OFF = 31
};

// Map for ArduinoNano thread commands, maps command strings to ArduinoCommands enum values 
// representing which pin to set high or low.
std::map<std::string, ArduinoCommands>& arduinoCommandMap()
{
    static std::map<std::string, ArduinoCommands> map;

    map = {
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
        {"D OFF 15",ArduinoCommands::DIODE_15_OFF},
        {"D ON 16", ArduinoCommands::DIODE_16_ON},
        {"D OFF 16", ArduinoCommands::DIODE_16_OFF}
    };

    return map;
}

// enum to make defining DAQManager commands easier
enum class DAQCommands
{
    START_DAQ = 0,
    STOP_DAQ = 1
};

// Map for DAQManager thread commands, maps command strings to DAQCommands enum values representing
// which command to carry out.
std::map<std::string, DAQCommands>& daqCommandMap()
{
    static std::map<std::string, DAQCommands> map;

    map = {
        {"START DAQ", DAQCommands::START_DAQ},
        {"STOP DAQ", DAQCommands::STOP_DAQ}
    };

    return map;
}

// ============================================================
// DiodeProgram
// ============================================================

// TODO
DiodeProgram::DiodeProgram()
{
    
}

// TODO
DiodeProgram::~DiodeProgram()
{
    
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
    daqManager.start_reading();

    std::cout << "DAQ startup successful" << std::endl;

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

            for (const auto& row : data)
            {
                for (size_t j = 0; j < row.size(); j+=2)
                {
                    std::cout << "CH"<< (j/2) <<": V = "<< row[j] << ", T = " << row[j+1] << "\n";
                }
            }
        }

        // update current temperatures and voltages vectors with most recent data, for now just filling in voltage values
        if (data.size() != 0)
        {
            std::lock_guard<std::mutex> lock_current_temperatures(currentTemperaturesMutex);
            std::lock_guard<std::mutex> lock_current_voltages(currentVoltagesMutex);
            for (size_t channel_iterator = 0; channel_iterator < currentVoltages.size(); ++channel_iterator)
            {
                currentVoltages[channel_iterator] = data.back()[channel_iterator * 2 + 1];
                currentTemperatures[channel_iterator] = data.back()[channel_iterator * 2 + 2];
            }
        }

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
        const std::string portName = "/dev/ttyUSB0";   // Linux example: "/dev/ttyUSB0"
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

// TODO
void DiodeProgram::mainAppLoop()
{
    // Initialisation?

    while(mainThreadRunning)
    {
        // check command queue and carry out commands
    }

    shutdownProcedure();
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

// TODO
bool DiodeProgram::shutdownProcedure()
{
    
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
        case ArduinoCommands::DIODE_1_ON:
        /* TODO: NOTE SINCE DIODE PINS 1 AND 2 NON-FUNCTIONAL, CURRENT VERSION HAS PINS 1 AND 2 
        SHORTED WITH 3, WHICH IS FUNCTIONAL  */
            arduinoNano.setPinHigh(3); 
            break;
        case ArduinoCommands::DIODE_1_OFF:
            arduinoNano.setPinLow(3); 
            break;
        case ArduinoCommands::DIODE_2_ON:
            arduinoNano.setPinHigh(3); 
            break;
        case ArduinoCommands::DIODE_2_OFF:
            arduinoNano.setPinLow(3); 
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
        case ArduinoCommands::DIODE_16_ON:
            arduinoNano.setPinHigh(16);
            break;
        case ArduinoCommands::DIODE_16_OFF:
            arduinoNano.setPinLow(16);
            break;
        default:
            std::cout << "Unknown Arduino command: " << command << std::endl;
    }
}

//TODO
void DiodeProgram::checkMainCommands()
{
    
}

//TODO
void DiodeProgram::carryOutMainCommand(std::string &command)
{
    
}

// ============================================================
// Public member functions
// ============================================================

void DiodeProgram::run()
{
    // run startup procedure, if it returns false, print error and exit
   if (startupProcedure() == false)
   {
    std::cerr << "Error during startup procedure. Exiting." << std::endl;
    return;
   };

   // start main app loop
   mainThreadRunning = true;
   mainThread = std::thread(&DiodeProgram::mainAppLoop, this);
}

//TODO
void DiodeProgram::quitApp()
{
    mainThreadRunning = false;
}