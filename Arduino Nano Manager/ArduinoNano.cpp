#include "ArduinoNano.h"

#include <sstream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <errno.h>
    #include <cstring>
#endif

// ============================================================
// Constructors
// ============================================================

ArduinoNano::ArduinoNano()
    : m_serial(),
      m_portName(),
      m_lastError(),
      m_pinStates()
{
    resetAllStoredPinStates();
}

ArduinoNano::ArduinoNano(const std::string& portName)
    : m_serial(),
      m_portName(portName),
      m_lastError(),
      m_pinStates()
{
    resetAllStoredPinStates();
}

// ============================================================
// Connection management
// ============================================================

bool ArduinoNano::connect(const std::string& portName, int baudRate)
{
    m_lastError.clear();
    m_portName = portName;

    // Open the underlying serial port.
    if (!m_serial.open(portName)) {
        return fail(m_serial.lastError());
    }

    // Configure the port for typical Arduino settings.
    // Most sketches use 9600 or 115200, but 9600 is the default here
    // because that matched your earlier code.
    if (!m_serial.configure(baudRate, 8, 1, 'N')) {
        return fail(m_serial.lastError());
    }

    // Give reads a predictable timeout so waiting logic behaves well.
    if (!m_serial.setReadTimeoutMs(500)) {
        return fail(m_serial.lastError());
    }

    // Once reconnected, our previous software-side pin cache should
    // not be trusted anymore. Reset it to Unknown.
    resetAllStoredPinStates();
    applyKnownStartupPinStates();

    return true;
}

void ArduinoNano::disconnect()
{
    m_serial.close();

    // Once the board is disconnected, software can no longer claim
    // to know the real pin states with confidence.
    resetAllStoredPinStates();
}

bool ArduinoNano::isConnected() const
{
    return m_serial.isOpen();
}

// ============================================================
// Device startup / synchronization
// ============================================================

void ArduinoNano::reset(int lowMs, int highMs)
{
    if (!isConnected()) {
        m_lastError = "Cannot reset Arduino Nano: device is not connected";
        return;
    }

#ifdef _WIN32
    // Many Arduino-compatible boards reset when DTR is toggled.
    // First drive DTR low, wait briefly, then drive it high again.
    if (!EscapeCommFunction(m_serial.nativeHandle(), CLRDTR)) {
        m_lastError = "Failed to clear DTR during reset";
        return;
    }

    Sleep(lowMs);

    if (!EscapeCommFunction(m_serial.nativeHandle(), SETDTR)) {
        m_lastError = "Failed to set DTR during reset";
        return;
    }

    Sleep(highMs);

#else
    // On Linux / POSIX systems, DTR is controlled through ioctl
    // on the serial file descriptor.
    int status = 0;

    if (ioctl(m_serial.nativeHandle(), TIOCMGET, &status) == -1) {
        m_lastError = "Failed to read modem control lines during reset: " +
                      std::string(std::strerror(errno));
        return;
    }

    // Drive DTR low
    status &= ~TIOCM_DTR;
    if (ioctl(m_serial.nativeHandle(), TIOCMSET, &status) == -1) {
        m_lastError = "Failed to clear DTR during reset: " +
                      std::string(std::strerror(errno));
        return;
    }

    usleep(lowMs * 1000);

    // Drive DTR high again
    status |= TIOCM_DTR;
    if (ioctl(m_serial.nativeHandle(), TIOCMSET, &status) == -1) {
        m_lastError = "Failed to set DTR during reset: " +
                      std::string(std::strerror(errno));
        return;
    }

    usleep(highMs * 1000);
#endif

    // After a hardware reset, the software-side cached pin states
    // should no longer be trusted until state is re-established.
    resetAllStoredPinStates();
    applyKnownStartupPinStates();
}

bool ArduinoNano::waitForReady(const std::string& readyMessage, int maxAttempts)
{
    if (!isConnected()) {
        return fail("Cannot wait for READY: device is not connected");
    }

    if (maxAttempts <= 0) {
        return fail("maxAttempts must be greater than zero");
    }

    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        std::string line = m_serial.readLine();

        if (!line.empty() && line == readyMessage) {
            return true;
        }
    }

    std::ostringstream oss;
    oss << "Timed out waiting for ready message: \"" << readyMessage << "\"";
    return fail(oss.str());
}

// ============================================================
// Pin control
// ============================================================

bool ArduinoNano::setPinHigh(int pinNumber)
{
    return setPinState(pinNumber, PinState::High);
}

bool ArduinoNano::setPinLow(int pinNumber)
{
    return setPinState(pinNumber, PinState::Low);
}

bool ArduinoNano::setPinState(int pinNumber, PinState state)
{
    if (!isConnected()) {
        return fail("Cannot set pin state: device is not connected");
    }

    if (!isValidPinNumber(pinNumber)) {
        return fail("Invalid pin number. Valid digital pins are 0 through 15.");
    }

    if (state != PinState::High && state != PinState::Low) {
        return fail("setPinState only accepts High or Low");
    }

    // Send the line-based command to the Arduino.
    if (!sendPinCommand(pinNumber, state)) {
        return false;
    }

    // If the command was sent successfully, update the software-side
    // cache of the pin state.
    //
    // NOTE:
    // This assumes a successful command send means the Arduino will
    // apply the change. If you later want stronger correctness, you
    // could require an "OK" response from the board before updating.
    updateStoredPinState(pinNumber, state);
    return true;
}

// ============================================================
// Getters
// ============================================================

const std::string& ArduinoNano::portName() const
{
    return m_portName;
}

std::string ArduinoNano::lastError() const
{
    // Prefer this class's own error message when present.
    // Otherwise return the lower-level SerialPort error.
    if (!m_lastError.empty()) {
        return m_lastError;
    }

    return m_serial.lastError();
}

ArduinoNano::PinState ArduinoNano::pinState(int pinNumber) const
{
    if (!isValidPinNumber(pinNumber)) {
        return PinState::Unknown;
    }

    return m_pinStates[static_cast<std::size_t>(pinNumber)];
}

const std::array<ArduinoNano::PinState, 16>& ArduinoNano::allPinStates() const
{
    return m_pinStates;
}

bool ArduinoNano::isPinHigh(int pinNumber) const
{
    return pinState(pinNumber) == PinState::High;
}

bool ArduinoNano::isPinLow(int pinNumber) const
{
    return pinState(pinNumber) == PinState::Low;
}

bool ArduinoNano::isPinStateKnown(int pinNumber) const
{
    return pinState(pinNumber) != PinState::Unknown;
}

// ============================================================
// Generic line I/O helpers
// ============================================================

bool ArduinoNano::sendCommand(const std::string& command)
{
    if (!isConnected()) {
        return fail("Cannot send command: device is not connected");
    }

    if (!m_serial.writeLine(command)) {
        return fail(m_serial.lastError());
    }

    return true;
}

std::string ArduinoNano::readLine()
{
    if (!isConnected()) {
        m_lastError = "Cannot read line: device is not connected";
        return {};
    }

    return m_serial.readLine();
}

std::string ArduinoNano::readAvailableText()
{
    if (!isConnected()) {
        m_lastError = "Cannot read text: device is not connected";
        return {};
    }

    return m_serial.readAvailableText();
}

// ============================================================
// Private helpers
// ============================================================

bool ArduinoNano::isValidPinNumber(int pinNumber) const
{
    return pinNumber >= 0 && pinNumber < pinCount();
}

bool ArduinoNano::fail(const std::string& message)
{
    m_lastError = message;
    return false;
}

bool ArduinoNano::sendPinCommand(int pinNumber, PinState state)
{
    // The PC application uses internal pin numbers 0..15.
    // The Arduino command protocol uses transmitted pin numbers 1..16
    // so that String::toInt() returning 0 on parse failure cannot be
    // confused with a valid transmitted pin number.
    const int transmittedPinNumber = pinNumber + 1;

    std::ostringstream command;
    command << "SET DIGITAL ";

    command << transmittedPinNumber;

    if (state == PinState::High) {
        command << " ON";
    } else if (state == PinState::Low) {
        command << " OFF";
    } else {
        return fail("Cannot send pin command with Unknown state");
    }

    

    if (!m_serial.writeLine(command.str())) {
        return fail(m_serial.lastError());
    }

    return true;
}

void ArduinoNano::updateStoredPinState(int pinNumber, PinState state)
{
    if (isValidPinNumber(pinNumber)) {
        m_pinStates[static_cast<std::size_t>(pinNumber)] = state;
    }
}

void ArduinoNano::resetAllStoredPinStates()
{
    for (auto& state : m_pinStates) {
        state = PinState::Unknown;
    }
}

void ArduinoNano::applyKnownStartupPinStates()
{
    // Match the Arduino sketch:
    // pins 2..15 are configured as outputs and driven LOW in setup().
    //
    // Pins 0 and 1 are left as Unknown because they are used by Serial.
    // If you later change the sketch startup behavior, update this
    // function to keep the PC-side cache consistent with the firmware.
    resetAllStoredPinStates();

    for (int pin = 2; pin <= 15; ++pin) {
        m_pinStates[static_cast<std::size_t>(pin)] = PinState::Low;
    }
}