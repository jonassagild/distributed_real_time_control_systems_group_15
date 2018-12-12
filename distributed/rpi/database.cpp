//
// database.cpp
//
// Jonas Åsnes Sagild 09.12.18
//
//

#include "database.hpp"

Database::Database() {
	time_last_restart = std::chrono::system_clock::now(); 
}


