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
	// illuminance
	int illuminance_1 = 10;
	std::mutex mux_illuminance_1;
	int illuminance_2 = 10;
	std::mutex mux_illuminance_2;
	
	// dimmings
	std::string dim_1 = "0";
	std::mutex mux_dim_1;
	std::string dim_2 = "0";
	std::mutex mux_dim_2;

	// lower bound
	std::string low_bound_1 = "0";
	std::mutex mux_low_bound_1;
	std::string low_bound_2 = "0";
	std::mutex mux_low_bound_2;

	// pwm 
	std::string pwm_1 = "0";
	std::mutex mux_pwm_1;
	std::string pwm_2 = "0";
	std::mutex mux_pwm_2;

	// time since last restart
	std::chrono::system_clock::time_point time_last_restart;
};

#endif /* database.hpp */
