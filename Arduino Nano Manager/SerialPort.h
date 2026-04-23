#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <string>
#include <vector>
#include <cstdint>

// ============================================================
// Cross-platform serial handle type
// ============================================================

#ifdef _WIN32
    #include <windows.h>
    using NativeSerialHandle = HANDLE;
#else
    using NativeSerialHandle = int;
#endif

class SerialPort
{
public:
    // --------------------------------------------------------
    // Constructor / destructor
    //
    // The destructor automatically closes the port if needed.
    // --------------------------------------------------------
    SerialPort();
    explicit SerialPort(const std::string& portName);
    ~SerialPort();

    // --------------------------------------------------------
    // Non-copyable
    //
    // A serial port object owns an OS resource, so copying it
    // would be dangerous / confusing.
    // --------------------------------------------------------
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    // --------------------------------------------------------
    // Movable
    //
    // Moving is allowed so ownership can be transferred.
    // --------------------------------------------------------
    SerialPort(SerialPort&& other) noexcept;
    SerialPort& operator=(SerialPort&& other) noexcept;

    // --------------------------------------------------------
    // Open / close / configure
    // --------------------------------------------------------
    bool open(const std::string& portName);
    void close();
    bool isOpen() const;

    // Configure basic serial settings.
    // Default is 9600 8N1, which matches your earlier code.
    bool configure(int baudRate = 9600,
                   int dataBits = 8,
                   int stopBits = 1,
                   char parity = 'N');

    // --------------------------------------------------------
    // I/O
    // --------------------------------------------------------

    // Write raw bytes
    bool writeBytes(const std::uint8_t* data, std::size_t size);

    // Write a string exactly as given
    bool writeString(const std::string& text);

    // Write a line, appending '\n'
    bool writeLine(const std::string& line);

    // Read up to maxBytes into a byte vector.
    // Returns however many bytes were actually read.
    std::vector<std::uint8_t> readBytes(std::size_t maxBytes);

    // Read whatever text is immediately available right now
    std::string readAvailableText();

    // Read until '\n'. Ignores '\r'.
    std::string readLine();

    // --------------------------------------------------------
    // Utility
    // --------------------------------------------------------
    const std::string& portName() const;
    std::string lastError() const;

    // Set read timeout in milliseconds.
    // This affects how long reads wait before returning "no data".
    bool setReadTimeoutMs(int timeoutMs);

        // Returns the underlying OS-specific serial handle.
    //
    // This is mainly intended for higher-level device wrappers
    // such as ArduinoNano when they need limited low-level control
    // like toggling DTR to reset the board.
    //
    // Callers should avoid doing general reads/writes directly with
    // this handle, because that would bypass the SerialPort class.
    NativeSerialHandle nativeHandle() const;

private:
    // Shared helper for constructors / move ops
    void resetMembers();

    // Convert a portable baud rate integer into platform-specific
    // settings. Implemented in the .cpp.
    bool applyPlatformConfig(int baudRate,
                             int dataBits,
                             int stopBits,
                             char parity);

    bool applyPlatformReadTimeout(int timeoutMs);

    static void sleepMs(int ms);

private:
    NativeSerialHandle m_handle;
    std::string m_portName;
    std::string m_lastError;
    int m_readTimeoutMs;
};

#endif // SERIAL_PORT_H