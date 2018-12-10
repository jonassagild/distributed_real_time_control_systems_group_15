// 
// session.cpp
//
// Jonas Åsnes Sagild 10.12.18
//

#include "session.hpp"


void session::handle_read(const error_code& ec, std::size_t sz) {
        if (!ec) {
            async_write(s, buffer(data, sz), boost::bind(&session::handle_write, this, _1, _2));
        } else {
            delete this;
        }
}

void session::handle_write(const error_code& ec, std::size_t sz) {
        if (!ec) {
            s.async_read_some(buffer(data, max_len), boost::bind(&session::handle_read, this, _1, _2));
        } else {
            delete this;
        }
}

session::session(io_service& io): s(io) {}

tcp::socket& session::socket() {
        return s;
}

void session::start() {
        s.async_read_some(buffer(data, max_len), boost::bind(&session::handle_read, this, _1, _2));
}

