#ifndef MINIMAX_NET_TRANSPORT_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_NET_TRANSPORT_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/net/socket.hpp"
#include "minimax/protocol/messages.hpp"
#include <vector>
#include <memory>

namespace minimax {
namespace net {

class Transport {
public:
    explicit Transport(Socket socket);

    virtual ~Transport();
    Transport(const Transport& other) = delete;
    Transport& operator=(const Transport& other) = delete;
    
    virtual bool send_message(const std::vector<uint8_t>& data) = 0;
    virtual bool receive_message(std::vector<uint8_t>& data) = 0;
    
    bool is_connected() const;
    void disconnect();

protected:
    Socket socket_;
    bool connected_;
};

}
}

#endif // MINIMAX_NET_TRANSPORT_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
