#include "DiodeProgram.h"

// ============================================================
// DiodeProgram
// ============================================================

DiodeProgram::DiodeProgram()
{
    
}

// ============================================================
// Private helper functions
// ============================================================

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
        checkDiodeCommands();

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
        checkArduinoCommands();

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

void DiodeProgram::mainAppLoop()
{

}

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
}

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

void DiodeProgram::checkDiodeCommands()
{
   
}

void DiodeProgram::carryOutDiodeCommand(std::string &command)
{
    
}

void DiodeProgram::checkArduinoCommands()
{
    
}

void DiodeProgram::carryOutArduinoCommand(std::string &command)
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

void DiodeProgram::quitApp()
{
    
}