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
	int illuminance = 0;
	std::mutex mux_illuminance;	

};

#endif /* database.hpp */
