#include "minimax/net/transport.hpp"
#include "minimax/common/logging.hpp"

namespace minimax {
namespace net {

Transport::Transport(Socket socket)
: socket_(std::move(socket))
, connected_(socket_.is_valid()) {}

Transport::~Transport() {}

bool Transport::is_connected() const
{
    return connected_;
}

void Transport::disconnect()
{
    socket_.close();
    connected_ = false;
}

}
}
