#include "minimax/net/transport_blocking.hpp"

#define BUFFER_SIZE (4096)

namespace minimax {
namespace net {

BlockingTransport::BlockingTransport(Socket socket)
: Transport(std::move(socket))
{
    recv_buffer_.reserve(BUFFER_SIZE);
}

bool BlockingTransport::send_message(const std::vector<uint8_t>& data)
{
    if (!connected_) {
        return false;
    }
    
    // Create 4-byte(protocol standard) header for TCP (Big Endian format)
    uint32_t length = static_cast<uint32_t>(data.size());
    uint8_t header[4];
    header[0] = (length >> 24) & 0xFF; // MSB
    header[1] = (length >> 16) & 0xFF;
    header[2] = (length >> 8) & 0xFF;
    header[3] = length & 0xFF; // LSB
    
    ssize_t sent = socket_.send(header, 4);
    if (sent != 4) {
        connected_ = false;
        return false;
    }
    
    sent = socket_.send(data.data(), data.size());
    if (sent != static_cast<ssize_t>(data.size())) {
        connected_ = false;
        return false;
    }
    
    return true;
}

bool BlockingTransport::receive_message(std::vector<uint8_t>& data)
{
    if (!connected_) {
        return false;
    }
    
    // Read Length
    uint8_t header[4];
    ssize_t received = socket_.receive(header, 4);
    if (received != 4) {
        connected_ = false;
        return false;
    }
    
    uint32_t length = (static_cast<uint32_t>(header[0]) << 24) | 
                     (static_cast<uint32_t>(header[1]) << 16) |
                     (static_cast<uint32_t>(header[2]) << 8) |
                     static_cast<uint32_t>(header[3]);
    
    data.resize(length);
    std::size_t offset = 0;
    
    // Length prefix read data
    while (offset < length) {
        received = socket_.receive(data.data() + offset,
                                  length - offset);
        if (received <= 0) {
            connected_ = false;
            return false;
        }
        offset += received;
    }
    
    return true;
}

}
}
