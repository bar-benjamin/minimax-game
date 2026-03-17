#ifndef MINIMAX_NET_SOCKET_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_NET_SOCKET_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include <string>
#include <cstddef>
#include <cstdint>
#include <sys/socket.h>

namespace minimax {
namespace net {

class Socket {
public:
    explicit Socket(int fd = -1);

    ~Socket();
    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    
    static Socket create_tcp();
    static Socket create_and_bind(uint16_t port);
    
    bool connect(const std::string& host, uint16_t port);
    void listen(int backlog = 10);
    Socket accept();
    
    ssize_t send(const void* data, std::size_t size);
    ssize_t receive(void* buffer, std::size_t size);
    
    void set_non_blocking(bool flag);
    void set_reuse_addr(bool flag);
    
    bool is_valid() const;
    int get_fd() const;
    void close();

private:
    int fd_;
};

}
}

#endif // MINIMAX_NET_SOCKET_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
