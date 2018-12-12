//
// database.hpp
//
// Jonas Åsnes Sagild 09.12.18
//
//

#ifndef database_hpp
#define database_hpp

#include <mutex>


class Database {
public:
	Database();
	
	// variables
	int illuminance_1 = 0;
	std::mutex mux_illuminance_1;
	int illuminance_2 = 0;
	std::mutex mux_illuminance_2;
		
	

	// time since last restart
	std::chrono::system_clock::time_point time_last_restart;
};

#endif /* database.hpp */
