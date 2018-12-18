// 
// session.cpp
//
// Jonas Åsnes Sagild 10.12.18
//

#include "session.hpp"
#include <string.h>

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
	switch(msgBuff[0]) {
		case 'g':
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

					// return elapsed seconds since last restart
	

					async_write(s, buffer(msgBuff, sz), boost::bind(&session::handle_write, this, _1, _2));						
					return;
				case 'l':
					// if 1 or 2, call async_write with correct value
					if (msgBuff[4] == '1') {
						// return illumininace 1
						_db->mux_illuminance_1.lock();
						std::string streng = std::to_string(_db->illuminance_1);
						_db->mux_illuminance_1.unlock();
						streng = "l 1 " + streng; 
						add_string_to_data(streng);
						async_write(s, buffer(data, strlen(data)), boost::bind(&session::handle_write, this, _1, _2));
						return;
					} else if (msgBuff[4] == '2') {
						// return illuminanc 2
						_db->mux_illuminance_2.lock();
						std::string streng = std::to_string(_db->illuminance_2);
						_db->mux_illuminance_2.unlock();
						streng = "l 2 " + streng; 
						add_string_to_data(streng);
						async_write(s, buffer(data, strlen(data)), boost::bind(&session::handle_write, this, _1, _2));
						return;
					}	
					
					async_write(s, buffer(msgBuff, sz), boost::bind(&session::handle_write, this, _1, _2));						
					return;
				case 'D':
					// return dim
					if (msgBuff[4] == '1') {
						// return dim 1
						_db->mux_dim_1.lock();
						std::string streng;
					   	streng = _db->dim_1;
						_db->mux_dim_1.unlock();
						add_string_to_data(streng);
						async_write(s, buffer(data, strlen(data)), boost::bind(&session::handle_write, this, _1, _2));
					} else if (msgBuff[4] == '2') {
						// return dim 2
						_db->mux_dim_2.lock();
						std::string streng;
					   	streng = _db->dim_2;
						_db->mux_dim_2.unlock();
						add_string_to_data(streng);
						async_write(s, buffer(data, strlen(data)), boost::bind(&session::handle_write, this, _1, _2));
					}
					break;
					return;
				case 's':
					if (msgBuff[4] == '1') {
						std::string streng;
						streng = "s 1 1";
						add_string_to_data(streng);
						async_write(s, buffer(data, strlen(data)), boost::bind(&session::handle_write, this, _1, _2));
					} else if (msgBuff [4] == '2') {
						std::string streng;
						streng = "s 2 1";
						add_string_to_data(streng);
						async_write(s, buffer(data, strlen(data)), boost::bind(&session::handle_write, this, _1, _2));
					}
					return;
				case 'd':
					if (msgBuff[4] == '1') {
						std::string streng;
						streng = "d 1 ";
						_db->mux_pwm_1.lock();
						streng += _db->pwm_1;
						_db->mux_pwm_1.unlock();
						add_string_to_data(streng);
						async_write(s, buffer(data, strlen(data)), boost::bind(&session::handle_write, this, _1, _2));
					} else if (msgBuff[4] == '2') {
						std::string streng;
						streng = "d 2 ";
						_db->mux_pwm_2.lock();
						streng += _db->pwm_2;
						_db->mux_pwm_2.unlock();
						add_string_to_data(streng);
						async_write(s, buffer(data, strlen(data)), boost::bind(&session::handle_write, this, _1, _2));
					}
					break;
				default:
					break;
			}
			break;
		case 'r':
			std::cout << "case 'r'\n";
			// TODO: implement
			break;	
		default:
			break;
	}
	std::cout << "\n under switch\n";
	
	s.async_read_some(buffer(data, max_len), boost::bind(&session::handle_read, this, _1, _2));
}

void session::add_string_to_data(std::string streng) {
	// TODO: copy data from streng to data, and then clear the rest of the data, and then add a new line
	// adds the characters in streng to data
	streng.copy(data, streng.length(), 0);
	// fills the rest of data with \0
	char *begin = &data[streng.length()];
	char *end = begin + sizeof(data) - streng.length()-1;
	std::fill(begin, end, '\0');
	// adds \n after last character in data
	data[streng.length()] = '\n';	
}

session::session(io_service& io, std::shared_ptr <Database> db ): s(io), _db(db) {
	
}

tcp::socket& session::socket() {
	return s;
}

void session::start() {
	s.async_read_some(buffer(data, max_len), boost::bind(&session::handle_read, this, _1, _2));
}

