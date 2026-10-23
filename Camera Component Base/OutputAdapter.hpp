#ifndef OUTPUT_ADAPTER_HPP
#define OUTPUT_ADAPTER_HPP

#include <vector>
#include <cstdint>

class OutputAdapter {
public:
    static std::vector<uint8_t> encodeMarkerData(int id, float zoom, float pitch, float yaw, float roll);
};

#endif