#include "OutputAdapter.hpp"
#include <string>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

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
    data.push_back(static_cast<uint8_t>( value        & 0xFF));
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

OutputAdapter::OutputAdapter(const std::string& hostname, uint16_t port)
    : hostname(hostname), port(port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "OutputAdapter: WSAStartup failed." << std::endl;
        return;
    }

    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET)
    {
        std::cerr << "OutputAdapter: Failed to create socket." << std::endl;
        WSACleanup();
        return;
    }

    socketHandle = static_cast<uintptr_t>(s);
    isOpen = true;
}

OutputAdapter::~OutputAdapter()
{
    if (isOpen)
    {
        closesocket(static_cast<SOCKET>(socketHandle));
        WSACleanup();
    }
}

bool OutputAdapter::sendMarkerData(int id, float zoom, float pitch, float yaw, float roll)
{
    if (!isOpen) return false;

    std::vector<uint8_t> markerData = encodeMarkerData(id, zoom, pitch, yaw, roll);

    // Info about which port and Ipv4 address to send the data to
    sockaddr_in target{};
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    if (inet_pton(AF_INET, hostname.c_str(), &target.sin_addr) != 1)
    {
        return false; // Invalid IP address
    }

    int sent = sendto(static_cast<SOCKET>(socketHandle),
                      reinterpret_cast<const char*>(markerData.data()),
                      static_cast<int>(markerData.size()),
                      0,
                      reinterpret_cast<sockaddr*>(&target),
                      sizeof(target));

    return sent == static_cast<int>(markerData.size());
}
