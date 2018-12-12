// 
// server.cpp
//
// Jonas Åsnes Sagild
//
// 


#include "server.hpp"

void server::start_accept() {
	session* new_sess = new session(io, _db);
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


server::server(io_service& io, short port, std::shared_ptr <Database> db): io(io), acceptor(io, tcp::endpoint(tcp::v4(), port)), _db(db) {
		start_accept();
}

