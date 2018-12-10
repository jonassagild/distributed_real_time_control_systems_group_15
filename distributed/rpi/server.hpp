//
// server.hpp
// 
// Jonas Åsnes Sagild 10.12.18
//

#ifndef server_hpp
#define server_hpp


#include "session.hpp"


class server {
    io_service& io;
    tcp::acceptor acceptor;

    void start_accept();

    void handle_accept(session* sess, const error_code& ec);

public:
    server(io_service& io, short port);
};

#endif /* server.hpp */
