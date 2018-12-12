//
// system.cpp
// 
// Jonas Åsnes Sagild 08.12.18
//
// g++ -std=c++11 -lpigpio -lrt -o system -pthread database.hpp database.cpp data_collection.cpp data_collection.hpp session.cpp session.hpp server.hpp server.cpp system.hpp system.cpp -lboost_system

#include "system.hpp"

// TODO: add two threads, one datacollector and one server. The variables needs to set in this process, and let the threads work on it. Try to see how it changes when using mutex. 



int main(){
	initialize_system();
	return 0;
}

void initialize_system() {
	std::shared_ptr<Database> db = std::make_shared<Database>();	
	DataCollector dataCollector = DataCollector(db);
	std::thread data_collector_thread (&DataCollector::read_values, dataCollector);
	
	io_service io;
	const char* port = "17000";	
	server s(io, std::atoi(port), db);
	std::thread server_thread ([&](){io.run();});

	data_collector_thread.join();
	server_thread.join();
	


	// TODO: setup db, datacollector, server etc
}

