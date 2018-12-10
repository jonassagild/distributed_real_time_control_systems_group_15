//
// session.hpp
//
// Jonas Åsnes Sagild 10.12.18
//

#ifndef session_hpp
#define session_hpp

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
using namespace boost::asio;
using boost::asio::ip::tcp;
using boost::system::error_code;

class session{
    tcp::socket s;
    enum {max_len = 1024};
    char data[max_len];

    void handle_read(const error_code& ec, std::size_t sz);

    void handle_write(const error_code& ec, std::size_t sz);

public:
    session(io_service& io);

    tcp::socket& socket();

    void start();
};



#endif /* session.hpp */
