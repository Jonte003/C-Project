#ifndef OUTPUT_ADAPTER_HPP
#define OUTPUT_ADAPTER_HPP

#include <vector>
#include <cstdint>
#include <string>

class OutputAdapter {
public:
    OutputAdapter(const std::string& hostname, uint16_t port);
    ~OutputAdapter();

    OutputAdapter(const OutputAdapter&) = delete;
    OutputAdapter& operator=(const OutputAdapter&) = delete;

    bool sendMarkerData(int id, float zoom, float pitch, float yaw, float roll);

    static std::vector<uint8_t> encodeMarkerData(int id, float zoom, float pitch, float yaw, float roll);

private:
    std::string hostname;
    uint16_t port;
    uintptr_t socketHandle = 0;
    bool isOpen = false;
};

#endif