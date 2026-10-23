#include "OutputAdapter.hpp"
#include <string>
#include <cstring>

static void appendPadString(std::vector<uint8_t>& data, const std::string& text)
{
    for (char c : text)
    {
        data.push_back(static_cast<uint8_t>(c));
    }

    data.push_back(0); // This is needed because OSC strings end with a null character

    while (data.size() % 4 != 0)
    {
        data.push_back(0); // Pad with null characters to make the size a multiple of 4
    }
}

static void appendUint32(std::vector<uint8_t>& data, uint32_t value)
{
    data.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    data.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    data.push_back(static_cast<uint8_t>((value >> 8)  & 0xFF));
    data.push_back(static_cast<uint8_t>(value         & 0xFF));
}

static void appendFloat(std::vector<uint8_t>& data, float value)
{
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits)); // Copy the float bits into an integer
    appendUint32(data, bits); // Append the integer representation of the float
}

std::vector<uint8_t> OutputAdapter::encodeMarkerData(int id, float zoom, float pitch, float yaw, float roll)
{
    std::vector<uint8_t> markerData;

    appendPadString(markerData, "/marker");
    appendPadString(markerData, ",iffff");
    appendUint32(markerData, static_cast<uint32_t>(id));
    appendFloat(markerData, zoom);
    appendFloat(markerData, pitch);
    appendFloat(markerData, yaw);
    appendFloat(markerData, roll);
    
    return markerData;
}
