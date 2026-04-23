#include "SerialPort.h"

#include <cstring>
#include <utility>
#include <sstream>

#ifndef _WIN32
    #include <fcntl.h>
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <errno.h>
#endif

// ============================================================
// Helpers for invalid handle values
// ============================================================

#ifdef _WIN32
    static NativeSerialHandle invalidSerialHandle()
    {
        return INVALID_HANDLE_VALUE;
    }
#else
    static NativeSerialHandle invalidSerialHandle()
    {
        return -1;
    }
#endif

// ============================================================
// Constructors / destructor
// ============================================================

SerialPort::SerialPort()
{
    resetMembers();
}

SerialPort::SerialPort(const std::string& portName)
{
    resetMembers();
    open(portName);
}

SerialPort::~SerialPort()
{
    close();
}

// ============================================================
// Move support
// ============================================================

SerialPort::SerialPort(SerialPort&& other) noexcept
{
    resetMembers();

    m_handle        = other.m_handle;
    m_portName      = std::move(other.m_portName);
    m_lastError     = std::move(other.m_lastError);
    m_readTimeoutMs = other.m_readTimeoutMs;

    other.m_handle = invalidSerialHandle();
    other.m_portName.clear();
    other.m_lastError.clear();
    other.m_readTimeoutMs = 500;
}

SerialPort& SerialPort::operator=(SerialPort&& other) noexcept
{
    if (this != &other) {
        close();

        m_handle        = other.m_handle;
        m_portName      = std::move(other.m_portName);
        m_lastError     = std::move(other.m_lastError);
        m_readTimeoutMs = other.m_readTimeoutMs;

        other.m_handle = invalidSerialHandle();
        other.m_portName.clear();
        other.m_lastError.clear();
        other.m_readTimeoutMs = 500;
    }

    return *this;
}

// ============================================================
// Public API
// ============================================================

bool SerialPort::open(const std::string& portName)
{
    close();
    m_lastError.clear();
    m_portName = portName;

#ifdef _WIN32
    m_handle = CreateFileA(
        portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (m_handle == invalidSerialHandle()) {
        m_lastError = "Failed to open serial port: " + portName;
        return false;
    }

#else
    m_handle = ::open(portName.c_str(), O_RDWR | O_NOCTTY);

    if (m_handle == invalidSerialHandle()) {
        m_lastError = "Failed to open serial port: " + portName +
                      " (" + std::string(std::strerror(errno)) + ")";
        return false;
    }

#endif

    // Apply the default timeout immediately so reads behave predictably.
    if (!applyPlatformReadTimeout(m_readTimeoutMs)) {
        close();
        return false;
    }

    return true;
}

void SerialPort::close()
{
    if (!isOpen()) {
        return;
    }

#ifdef _WIN32
    CloseHandle(m_handle);
#else
    ::close(m_handle);
#endif

    m_handle = invalidSerialHandle();
}

bool SerialPort::isOpen() const
{
    return m_handle != invalidSerialHandle();
}

bool SerialPort::configure(int baudRate, int dataBits, int stopBits, char parity)
{
    if (!isOpen()) {
        m_lastError = "Cannot configure: serial port is not open";
        return false;
    }

    return applyPlatformConfig(baudRate, dataBits, stopBits, parity);
}

bool SerialPort::writeBytes(const std::uint8_t* data, std::size_t size)
{
    if (!isOpen()) {
        m_lastError = "Cannot write: serial port is not open";
        return false;
    }

    if (data == nullptr && size > 0) {
        m_lastError = "Cannot write: null data pointer";
        return false;
    }

#ifdef _WIN32
    DWORD bytesWritten = 0;

    if (!WriteFile(m_handle,
                   data,
                   static_cast<DWORD>(size),
                   &bytesWritten,
                   nullptr)) {
        m_lastError = "WriteFile failed";
        return false;
    }

    if (bytesWritten != size) {
        m_lastError = "Partial write";
        return false;
    }

#else
    ssize_t bytesWritten = ::write(m_handle, data, size);

    if (bytesWritten < 0) {
        m_lastError = "write() failed: " + std::string(std::strerror(errno));
        return false;
    }

    if (static_cast<std::size_t>(bytesWritten) != size) {
        m_lastError = "Partial write";
        return false;
    }

    // Wait until queued output has actually been transmitted.
    tcdrain(m_handle);
#endif

    return true;
}

bool SerialPort::writeString(const std::string& text)
{
    return writeBytes(reinterpret_cast<const std::uint8_t*>(text.data()), text.size());
}

bool SerialPort::writeLine(const std::string& line)
{
    return writeString(line + "\n");
}

std::vector<std::uint8_t> SerialPort::readBytes(std::size_t maxBytes)
{
    std::vector<std::uint8_t> buffer;

    if (!isOpen()) {
        m_lastError = "Cannot read: serial port is not open";
        return buffer;
    }

    if (maxBytes == 0) {
        return buffer;
    }

    buffer.resize(maxBytes);

#ifdef _WIN32
    DWORD bytesRead = 0;

    if (!ReadFile(m_handle,
                  buffer.data(),
                  static_cast<DWORD>(maxBytes),
                  &bytesRead,
                  nullptr)) {
        m_lastError = "ReadFile failed";
        buffer.clear();
        return buffer;
    }

    buffer.resize(static_cast<std::size_t>(bytesRead));

#else
    ssize_t bytesRead = ::read(m_handle, buffer.data(), maxBytes);

    if (bytesRead < 0) {
        m_lastError = "read() failed: " + std::string(std::strerror(errno));
        buffer.clear();
        return buffer;
    }

    buffer.resize(static_cast<std::size_t>(bytesRead));
#endif

    return buffer;
}

std::string SerialPort::readAvailableText()
{
    if (!isOpen()) {
        m_lastError = "Cannot read: serial port is not open";
        return {};
    }

    std::string result;
    char chunk[256];

    while (true) {
#ifdef _WIN32
        DWORD bytesRead = 0;

        if (!ReadFile(m_handle, chunk, sizeof(chunk) - 1, &bytesRead, nullptr)) {
            m_lastError = "ReadFile failed";
            break;
        }

        if (bytesRead == 0) {
            break;
        }

        chunk[bytesRead] = '\0';

#else
        ssize_t bytesRead = ::read(m_handle, chunk, sizeof(chunk) - 1);

        if (bytesRead < 0) {
            m_lastError = "read() failed: " + std::string(std::strerror(errno));
            break;
        }

        if (bytesRead == 0) {
            break;
        }

        chunk[bytesRead] = '\0';
#endif

        result += chunk;
    }

    return result;
}

std::string SerialPort::readLine()
{
    if (!isOpen()) {
        m_lastError = "Cannot read line: serial port is not open";
        return {};
    }

    std::string line;
    char c = '\0';

    while (true) {
#ifdef _WIN32
        DWORD bytesRead = 0;

        if (!ReadFile(m_handle, &c, 1, &bytesRead, nullptr)) {
            m_lastError = "ReadFile failed while reading line";
            break;
        }

        if (bytesRead == 0) {
            // Timeout / no byte available yet
            continue;
        }

#else
        ssize_t bytesRead = ::read(m_handle, &c, 1);

        if (bytesRead < 0) {
            m_lastError = "read() failed while reading line: " +
                          std::string(std::strerror(errno));
            break;
        }

        if (bytesRead == 0) {
            // Timeout / no byte available yet
            continue;
        }
#endif

        if (c == '\n') {
            break;
        }

        if (c != '\r') {
            line += c;
        }
    }

    return line;
}

const std::string& SerialPort::portName() const
{
    return m_portName;
}

std::string SerialPort::lastError() const
{
    return m_lastError;
}

bool SerialPort::setReadTimeoutMs(int timeoutMs)
{
    if (timeoutMs < 0) {
        m_lastError = "Read timeout must be >= 0";
        return false;
    }

    m_readTimeoutMs = timeoutMs;

    if (!isOpen()) {
        // Save the timeout for later; not an error.
        return true;
    }

    return applyPlatformReadTimeout(timeoutMs);
}

NativeSerialHandle SerialPort::nativeHandle() const
{
    return m_handle;
}

// ============================================================
// Private helpers
// ============================================================

void SerialPort::resetMembers()
{
    m_handle = invalidSerialHandle();
    m_portName.clear();
    m_lastError.clear();
    m_readTimeoutMs = 500;
}

void SerialPort::sleepMs(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

bool SerialPort::applyPlatformReadTimeout(int timeoutMs)
{
    if (!isOpen()) {
        m_lastError = "Cannot set timeout: serial port is not open";
        return false;
    }

#ifdef _WIN32
    COMMTIMEOUTS timeouts = {};

    // This is a simple, practical timeout configuration for synchronous reads.
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = timeoutMs;
    timeouts.ReadTotalTimeoutMultiplier = 0;

    if (!SetCommTimeouts(m_handle, &timeouts)) {
        m_lastError = "Failed to set serial timeouts";
        return false;
    }

    return true;

#else
    struct termios tty {};

    if (tcgetattr(m_handle, &tty) != 0) {
        m_lastError = "Failed to get serial attributes: " +
                      std::string(std::strerror(errno));
        return false;
    }

    // VMIN = 0 means return as soon as any data is available,
    // or return 0 when the timeout expires.
    tty.c_cc[VMIN] = 0;

    // VTIME is in deciseconds on POSIX systems.
    // Round milliseconds up to the next 100 ms step.
    int deciseconds = (timeoutMs + 99) / 100;
    if (deciseconds < 0) {
        deciseconds = 0;
    }
    if (deciseconds > 255) {
        deciseconds = 255;
    }

    tty.c_cc[VTIME] = static_cast<cc_t>(deciseconds);

    if (tcsetattr(m_handle, TCSANOW, &tty) != 0) {
        m_lastError = "Failed to set serial timeout: " +
                      std::string(std::strerror(errno));
        return false;
    }

    return true;
#endif
}

bool SerialPort::applyPlatformConfig(int baudRate,
                                     int dataBits,
                                     int stopBits,
                                     char parity)
{
#ifdef _WIN32
    DCB dcb = {};
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(m_handle, &dcb)) {
        m_lastError = "Failed to get serial port state";
        return false;
    }

    // Only a few baud rates are commonly used. Windows accepts integer baud too,
    // but using the passed value directly is fine here.
    dcb.BaudRate = static_cast<DWORD>(baudRate);
    dcb.ByteSize = static_cast<BYTE>(dataBits);

    // Stop bits
    if (stopBits == 1) {
        dcb.StopBits = ONESTOPBIT;
    } else if (stopBits == 2) {
        dcb.StopBits = TWOSTOPBITS;
    } else {
        m_lastError = "Unsupported stop bits value";
        return false;
    }

    // Parity
    switch (parity) {
        case 'N':
        case 'n':
            dcb.Parity = NOPARITY;
            break;
        case 'E':
        case 'e':
            dcb.Parity = EVENPARITY;
            break;
        case 'O':
        case 'o':
            dcb.Parity = ODDPARITY;
            break;
        default:
            m_lastError = "Unsupported parity value";
            return false;
    }

    if (!SetCommState(m_handle, &dcb)) {
        m_lastError = "Failed to set serial port state";
        return false;
    }

    return applyPlatformReadTimeout(m_readTimeoutMs);

#else
    struct termios tty {};

    if (tcgetattr(m_handle, &tty) != 0) {
        m_lastError = "Failed to get serial attributes: " +
                      std::string(std::strerror(errno));
        return false;
    }

    // Map integer baud rate to POSIX baud constant
    speed_t speed;
    switch (baudRate) {
        case 9600:   speed = B9600; break;
        case 19200:  speed = B19200; break;
        case 38400:  speed = B38400; break;
        case 57600:  speed = B57600; break;
        case 115200: speed = B115200; break;
        default:
            m_lastError = "Unsupported baud rate on this POSIX implementation";
            return false;
    }

    if (cfsetispeed(&tty, speed) != 0 || cfsetospeed(&tty, speed) != 0) {
        m_lastError = "Failed to set baud rate: " +
                      std::string(std::strerror(errno));
        return false;
    }

    // Data bits
    tty.c_cflag &= ~CSIZE;
    switch (dataBits) {
        case 5: tty.c_cflag |= CS5; break;
        case 6: tty.c_cflag |= CS6; break;
        case 7: tty.c_cflag |= CS7; break;
        case 8: tty.c_cflag |= CS8; break;
        default:
            m_lastError = "Unsupported data bits value";
            return false;
    }

    // Stop bits
    if (stopBits == 1) {
        tty.c_cflag &= ~CSTOPB;
    } else if (stopBits == 2) {
        tty.c_cflag |= CSTOPB;
    } else {
        m_lastError = "Unsupported stop bits value";
        return false;
    }

    // Parity
    switch (parity) {
        case 'N':
        case 'n':
            tty.c_cflag &= ~PARENB;
            break;
        case 'E':
        case 'e':
            tty.c_cflag |= PARENB;
            tty.c_cflag &= ~PARODD;
            break;
        case 'O':
        case 'o':
            tty.c_cflag |= PARENB;
            tty.c_cflag |= PARODD;
            break;
        default:
            m_lastError = "Unsupported parity value";
            return false;
    }

    // Raw mode: no line editing, no echo, no special signal chars
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // No software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // No CR/LF rewriting
    tty.c_iflag &= ~(INLCR | ICRNL | IGNCR);

    // Raw output
    tty.c_oflag &= ~OPOST;

    // Receiver enabled, ignore modem control lines
    tty.c_cflag |= CREAD | CLOCAL;

    // Timeout settings
    tty.c_cc[VMIN] = 0;
    int deciseconds = (m_readTimeoutMs + 99) / 100;
    if (deciseconds < 0) {
        deciseconds = 0;
    }
    if (deciseconds > 255) {
        deciseconds = 255;
    }
    tty.c_cc[VTIME] = static_cast<cc_t>(deciseconds);

    if (tcsetattr(m_handle, TCSANOW, &tty) != 0) {
        m_lastError = "Failed to set serial attributes: " +
                      std::string(std::strerror(errno));
        return false;
    }

    return true;
#endif
}