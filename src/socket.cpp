#include "minimax/net/socket.hpp"
#include "minimax/common/logging.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <stdexcept>

namespace minimax {
namespace net {

Socket::Socket(int fd)
: fd_(fd)
{
}

Socket::~Socket()
{
    close();
}

Socket::Socket(Socket&& other) noexcept 
    : fd_(other.fd_) 
{
    other.fd_ = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept 
{
    if (this != &other) {
        if (fd_ >= 0) {
            close();
        }
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

Socket Socket::create_tcp()
{
    int fd = socket(AF_INET,     // IPv4 addresses
                    SOCK_STREAM, // TCP 
                    0);          // Pick standard protocol for above combo (TCP)

    if (fd < 0) {
        throw std::runtime_error("Error: failed to create socket");
    }
    return Socket(fd);
}

Socket Socket::create_and_bind(uint16_t port)
{
    Socket sock = create_tcp();

    // Prevents address is already in use bug
    sock.set_reuse_addr(true);              
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port); // Convert port to network byte order (big-endian)
    
    int bind_error = bind(sock.fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (bind_error < 0) {
        throw std::runtime_error("Error: failed to bind socket");
    }
    
    return sock;
}

bool Socket::connect(const std::string& host, uint16_t port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    // Convert IPv4 string to binary form 
    int conversion_result = inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    if (conversion_result <= 0) {
        return false;
    }
    
    return ::connect(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) >= 0;
}

void Socket::listen(int backlog)
{
    if (::listen(fd_, backlog) < 0) {
        throw std::runtime_error("Error: failed to listen on socket");
    }
}

Socket Socket::accept()
{
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);
    
    int client_fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&addr), &addr_len);
    if (client_fd < 0) {
        throw std::runtime_error("Error: failed to accept connection");
    }
    
    return Socket(client_fd);
}

ssize_t Socket::send(const void* data, std::size_t size)
{
    return ::send(fd_, data, size, MSG_NOSIGNAL);
}

ssize_t Socket::receive(void* buffer, std::size_t size)
{
    return ::recv(fd_, buffer, size, 0);
}

void Socket::set_non_blocking(bool flag)
{
    int flags = fcntl(fd_, F_GETFL, 0);

    if (flag) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    fcntl(fd_, F_SETFL, flags);
}

void Socket::set_reuse_addr(bool flag)
{
    int opt = flag ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

bool Socket::is_valid() const
{
    return fd_ >= 0;
}

int Socket::get_fd() const
{
    return fd_;
}

void Socket::close()
{
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

}
}
