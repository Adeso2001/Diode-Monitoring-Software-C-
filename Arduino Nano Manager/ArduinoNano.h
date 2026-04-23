#ifndef ARDUINO_NANO_H
#define ARDUINO_NANO_H

#include "SerialPort.h"

#include <array>
#include <string>

// ============================================================
// ArduinoNano
//
// High-level wrapper around a SerialPort for communicating with
// an Arduino Nano (or similar Arduino board) using a simple
// line-based text protocol.
//
// This class is responsible for:
//   - opening/configuring the serial connection
//   - resetting the Arduino when requested
//   - waiting for a "READY" message from the board
//   - sending pin-control commands
//   - storing the software-known state of digital pins 0-15
//
// IMPORTANT:
// The stored pin states are the class's best known states based
// on successful commands sent by this program. They are NOT true
// hardware readback unless your Arduino firmware explicitly sends
// back state information that you parse and apply.
// ============================================================

class ArduinoNano
{
public:
    // --------------------------------------------------------
    // PinState
    //
    // Represents the software-known state of a pin.
    //
    // Unknown:
    //   The program does not yet know the pin's current state.
    //
    // Low / High:
    //   The last successfully commanded state, or a state set
    //   from parsed device feedback if you later add that logic.
    // --------------------------------------------------------
    enum class PinState
    {
        Unknown,
        Low,
        High
    };

public:
    // --------------------------------------------------------
    // Constructors / destructor
    // --------------------------------------------------------
    ArduinoNano();
    explicit ArduinoNano(const std::string& portName);
    ~ArduinoNano() = default;

    // --------------------------------------------------------
    // Non-copyable
    //
    // This class owns a SerialPort, so copying is disabled.
    // --------------------------------------------------------
    ArduinoNano(const ArduinoNano&) = delete;
    ArduinoNano& operator=(const ArduinoNano&) = delete;

    // --------------------------------------------------------
    // Movable
    // --------------------------------------------------------
    ArduinoNano(ArduinoNano&& other) noexcept = default;
    ArduinoNano& operator=(ArduinoNano&& other) noexcept = default;

    // --------------------------------------------------------
    // Connection management
    //
    // connect():
    //   Opens the serial port, configures it, and optionally
    //   stores the chosen port name for future reference.
    //
    // disconnect():
    //   Closes the serial connection.
    //
    // isConnected():
    //   Returns true if the serial port is currently open.
    // --------------------------------------------------------
    bool connect(const std::string& portName, int baudRate = 9600);
    void disconnect();
    bool isConnected() const;

    // --------------------------------------------------------
    // Device startup / synchronization
    //
    // reset():
    //   Toggles DTR in an Arduino-style way to reset the board.
    //
    // waitForReady():
    //   Waits until the board sends a line exactly equal to the
    //   expected ready message, usually "READY".
    //
    // NOTE:
    // The matching Arduino sketch must actually print the ready
    // message for waitForReady() to succeed.
    // --------------------------------------------------------
    void reset(int lowMs = 100, int highMs = 100);
    bool waitForReady(const std::string& readyMessage = "READY",
                      int maxAttempts = 100);

    // --------------------------------------------------------
    // Pin control
    //
    // setPinHigh(pin):
    //   Sends a command to set the pin HIGH and updates the
    //   stored pin state if the command succeeds.
    //
    // setPinLow(pin):
    //   Sends a command to set the pin LOW and updates the
    //   stored pin state if the command succeeds.
    //
    // setPinState(pin, state):
    //   Convenience function for choosing High or Low.
    //
    // These functions expect the Arduino firmware to understand
    // commands of the form:
    //   SET PIN <pinNumber> HIGH
    //   SET PIN <pinNumber> LOW
    // --------------------------------------------------------
    bool setPinHigh(int pinNumber);
    bool setPinLow(int pinNumber);
    bool setPinState(int pinNumber, PinState state);

    // --------------------------------------------------------
    // Getters
    // --------------------------------------------------------

    // Returns the configured serial port name, e.g. "COM5" or
    // "/dev/ttyACM0".
    const std::string& portName() const;

    // Returns the last error message from this class or the
    // underlying SerialPort wrapper.
    std::string lastError() const;

    // Returns the stored state of a single pin.
    PinState pinState(int pinNumber) const;

    // Returns all stored pin states at once.
    const std::array<PinState, 16>& allPinStates() const;

    // Returns true if the class currently believes the pin is High.
    bool isPinHigh(int pinNumber) const;

    // Returns true if the class currently believes the pin is Low.
    bool isPinLow(int pinNumber) const;

    // Returns true if the pin has a known state (Low or High).
    bool isPinStateKnown(int pinNumber) const;

    // Optional utility: returns the number of supported pins.
    static constexpr int pinCount()
    {
        return 16;
    }

    // --------------------------------------------------------
    // Generic line I/O helpers
    //
    // These are exposed in case the larger program wants to use
    // the ArduinoNano class for additional board-specific text
    // commands beyond pin control.
    //
    // If you want a stricter interface later, you could remove
    // these from public access and wrap all commands in dedicated
    // member functions instead.
    // --------------------------------------------------------
    bool sendCommand(const std::string& command);
    std::string readLine();
    std::string readAvailableText();

private:
    // --------------------------------------------------------
    // Internal helpers
    // --------------------------------------------------------

    // Returns true if pinNumber is in the valid Nano digital pin
    // range handled by this class: 0 through 15 inclusive.
    bool isValidPinNumber(int pinNumber) const;

    // Sets the last error string and returns false, which is
    // convenient for guard clauses in bool-returning functions.
    bool fail(const std::string& message);

    // Builds and sends the protocol command for a pin-state change.
    bool sendPinCommand(int pinNumber, PinState state);

    // Updates the stored software-known pin state.
    void updateStoredPinState(int pinNumber, PinState state);

    // Resets all stored pin states to Unknown.
    void resetAllStoredPinStates();

    // Sets cached states to match the sketch's startup behavior.
    void applyKnownStartupPinStates();

private:
    // Underlying serial communication object.
    SerialPort m_serial;

    // Cached port name for getters / diagnostics.
    std::string m_portName;

    // Most recent error message for this class.
    std::string m_lastError;

    // Software-known state cache for digital pins 0-15.
    std::array<PinState, 16> m_pinStates;
};

#endif // ARDUINO_NANO_H