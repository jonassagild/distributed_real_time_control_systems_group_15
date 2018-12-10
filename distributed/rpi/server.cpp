// 
// server.cpp
//
// Jonas Åsnes Sagild
//
// 


#include "server.hpp"

//int main(int argc, char* argv[]) {
//    if (argc != 2) {
//        std::cerr << "usage: server <port> \n";
//        return 1;
//    }
//
//	
//    io_service io;
//    server s(io, std::atoi(argv[1]));
//    io.run();
//    return 0;
//}


 
void server::start_accept() {
	session* new_sess = new session(io);
        acceptor.async_accept(new_sess -> socket(), boost::bind(&server::handle_accept, this, new_sess, _1)); 
}

void server::handle_accept(session* sess, const error_code& ec) {
	if (!ec) {
            sess -> start();
        } else {
            delete sess;
        }
	start_accept();
}


server::server(io_service& io, short port): io(io), acceptor(io, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
}

