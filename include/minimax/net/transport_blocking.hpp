#ifndef MINIMAX_NET_TRANSPORT_BLOCKING_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_NET_TRANSPORT_BLOCKING_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/net/transport.hpp"


namespace minimax {
namespace net {

class BlockingTransport : public Transport {
public:
    explicit BlockingTransport(Socket socket);
    
    bool send_message(const std::vector<uint8_t>& data) override;
    bool receive_message(std::vector<uint8_t>& data) override;

private:
    std::vector<uint8_t> recv_buffer_;
};

}
}

#endif // MINIMAX_NET_TRANSPORT_BLOCKING_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e