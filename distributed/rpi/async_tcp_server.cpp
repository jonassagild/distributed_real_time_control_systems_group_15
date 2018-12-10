#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
using namespace boost::asio;
using boost::asio::ip::tcp;
using boost::system::error_code;

// To compile: g++ -std=c++11 -lboost_system -lpthread -o async_tcp_server async_tcp_server.cpp

class session{
    tcp::socket s;
    enum {max_len = 1024};
    char data[max_len];

    void handle_read(const error_code& ec, std::size_t sz) {
        if (!ec) {
            async_write(s, buffer(data, sz), boost::bind(&session::handle_write, this, _1, _2));
        } else {
            delete this;
        }
    }

    void handle_write(const error_code& ec, std::size_t sz) {
        if (!ec) {
            s.async_read_some(buffer(data, max_len), boost::bind(&session::handle_read, this, _1, _2));
        } else {
            delete this;
        }
    }

public:
    session(io_service& io): s(io) {}

    tcp::socket& socket() {
        return s;
    }

    void start() {
        s.async_read_some(buffer(data, max_len), boost::bind(&session::handle_read, this, _1, _2));
    }

};

class server {
    io_service& io;
    tcp::acceptor acceptor;

    void start_accept() {
        session* new_sess = new session(io);
        acceptor.async_accept(new_sess -> socket(), boost::bind(&server::handle_accept, this, new_sess, _1));
    }

    void handle_accept(session* sess, const error_code& ec) {
        if (!ec) {
            sess -> start();
        } else {
            delete sess;
        }
        start_accept();
    }

public:
    server(io_service& io, short port): io(io), acceptor(io, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: server <port> \n";
        return 1;
    }

    io_service io;
    server s(io, std::atoi(argv[1]));
    io.run();
    return 0;
}

