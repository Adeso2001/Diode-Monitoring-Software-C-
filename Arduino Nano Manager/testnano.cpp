#include "ArduinoNano.h"

#include <iostream>
#include <thread>
#include <chrono>

// ------------------------------------------------------------
// Small helper for readable pin-state printing
// ------------------------------------------------------------
std::string pinStateToString(ArduinoNano::PinState state)
{
    switch (state) {
        case ArduinoNano::PinState::Unknown:
            return "Unknown";
        case ArduinoNano::PinState::Low:
            return "Low";
        case ArduinoNano::PinState::High:
            return "High";
        default:
            return "Invalid";
    }
}

int main()
{
#ifdef _WIN32
    // Change this to the COM port your Nano actually uses.
    const std::string portName = "COM5";
#else
    // Change this if needed, e.g. /dev/ttyUSB0
    const std::string portName = "/dev/ttyACM0";
#endif

    // --------------------------------------------------------
    // Create the device object
    // --------------------------------------------------------
    ArduinoNano nano;

    std::cout << "Connecting to Arduino Nano on " << portName << "...\n";

    if (!nano.connect(portName, 9600)) {
        std::cerr << "Connect failed: " << nano.lastError() << "\n";
        return 1;
    }

    std::cout << "Connected.\n";

    // --------------------------------------------------------
    // Reset the board and wait for the startup READY message
    // --------------------------------------------------------
    std::cout << "Resetting board...\n";
    nano.reset();

    std::cout << "Waiting for READY...\n";
    if (!nano.waitForReady("READY", 100)) {
        std::cerr << "READY wait failed: " << nano.lastError() << "\n";
        return 1;
    }

    std::cout << "Board reported READY.\n";

    // --------------------------------------------------------
    // Choose a safe test pin
    //
    // Pin 13 is a good choice because many Arduino boards have
    // a built-in LED attached to it.
    // --------------------------------------------------------
    const int testPin = 13;

    std::cout << "\nTesting pin " << testPin << "...\n";

    // --------------------------------------------------------
    // Set pin HIGH
    // --------------------------------------------------------
    std::cout << "Setting pin " << testPin << " HIGH...\n";

    if (!nano.setPinHigh(testPin)) {
        std::cerr << "setPinHigh failed: " << nano.lastError() << "\n";
        return 1;
    }

    // Give the Arduino a brief moment to process and reply
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Read back any text the Arduino printed
    {
        std::string response = nano.readAvailableText();
        if (!response.empty()) {
            std::cout << "Arduino response after HIGH:\n" << response << "\n";
        }
    }

    std::cout << "Stored state for pin " << testPin << ": "
              << pinStateToString(nano.pinState(testPin)) << "\n";

    if (!nano.isPinHigh(testPin)) {
        std::cerr << "Unexpected stored state: pin should be High.\n";
        return 1;
    }

    // --------------------------------------------------------
    // Leave it high briefly so you can observe the board LED
    // if using pin 13
    // --------------------------------------------------------
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // --------------------------------------------------------
    // Set pin LOW
    // --------------------------------------------------------
    std::cout << "Setting pin " << testPin << " LOW...\n";

    if (!nano.setPinLow(testPin)) {
        std::cerr << "setPinLow failed: " << nano.lastError() << "\n";
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    {
        std::string response = nano.readAvailableText();
        if (!response.empty()) {
            std::cout << "Arduino response after LOW:\n" << response << "\n";
        }
    }

    std::cout << "Stored state for pin " << testPin << ": "
              << pinStateToString(nano.pinState(testPin)) << "\n";

    if (!nano.isPinLow(testPin)) {
        std::cerr << "Unexpected stored state: pin should be Low.\n";
        return 1;
    }

    // --------------------------------------------------------
    // Print all stored pin states so you can verify that the
    // class container is behaving as expected.
    // --------------------------------------------------------
    std::cout << "\nCurrent stored pin states:\n";
    for (int pin = 0; pin < ArduinoNano::pinCount(); ++pin) {
        std::cout << "  Pin " << pin << ": "
                  << pinStateToString(nano.pinState(pin)) << "\n";
    }

    // --------------------------------------------------------
    // Clean disconnect
    // --------------------------------------------------------
    nano.disconnect();
    std::cout << "\nTest completed successfully.\n";

    return 0;
}