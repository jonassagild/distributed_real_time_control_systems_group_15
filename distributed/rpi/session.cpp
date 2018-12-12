// 
// session.cpp
//
// Jonas Åsnes Sagild 10.12.18
//

#include "session.hpp"


void session::handle_read(const error_code& ec, std::size_t sz) {
	if (!ec) {
		// todo: call handle_command
		handle_command(data, strlen(data));
		// async_write(s, buffer(data, sz), boost::bind(&session::handle_write, this, _1, _2));
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

void session::write_data(std::size_t sz, char *message) {
	async_write(s, buffer(message, sz), boost::bind(&session::handle_write, this, _1, _2));		
}

void session::handle_command(char msgBuff[], int sz) {
	//std::cout << msgBuff;;
	std::cout << "nummer 0: " + std::string(1, msgBuff[0]) + "\n";
	std::cout << "nummer 1: " + std::string(1, msgBuff[1]) + "\n";
	std::cout << "nummer 2: " + std::string(1, msgBuff[2]) + "\n";
	std::cout << "nummer 3: " + std::string(1, msgBuff[3]) + "\n";
	std::cout << "nummer 4: " + std::string(1, msgBuff[4]) + "\n";
	switch(msgBuff[0]) {
		std::cout << "inni forste switch";
		case 'g':
			std::cout << "case 'g'";
			// add new switch to check what to be returned	
			switch(msgBuff[2]) {
				case 't':
					// TODO: implement handle to return time_last_restart
					// TODO: check the sz of the buffer, 4 is just a random number
					// std::size_t size = 4; // use strlen(chararray) to find size
					// async_write(s, buffer(_db->time_last_restart.str(), size),
					//		[this] (const error_code& ec, std::size_t sz){
					//			handle_read(ec, sz);	
					//		});						
					return;
				case 'l':
					// if 1 or 2, call async_write with correct value
					if (msgBuff[4] == '1') {
						// return illumininace 1
						// call async_write with correct input (handle_write) to write to user
					} else if (msgBuff[4] == '2') {
						std::cout << "den gaar inn rett";
						// return illuminanc 2
						// call async_write with handle_write
						// async_write(s, buffer(message, sz), boost::bind(&session::handle_write, this, _1, _2));
						_db->mux_illuminance_2.lock();
						std::string streng = std::to_string(_db->illuminance_2);
						const char *message = streng.c_str();
						_db->mux_illuminance_2.unlock();
						async_write(s, buffer(message, std::strlen(message)), boost::bind(&session::handle_write, this, _1, _2));
					}	
					
					return;
				
					default:
						break;
			}
			break;
		case 'r':
			// TODO: implement
			break;	
		default:
			break;
	}
	std::cout << "\n under switch";
	
	s.async_read_some(buffer(data, max_len), boost::bind(&session::handle_read, this, _1, _2));
}


session::session(io_service& io, std::shared_ptr <Database> db ): s(io), _db(db) {
	
}

tcp::socket& session::socket() {
	return s;
}

void session::start() {
	s.async_read_some(buffer(data, max_len), boost::bind(&session::handle_read, this, _1, _2));
}

