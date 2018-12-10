//
// data_collection.hpp
//
// Jonas Åsnes Sagild 09.12.18
//
//

#ifndef data_collection_hpp
#define data_collection_hpp

#include <stdio.h>
#include <pigpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "database.hpp"
#include <iostream>
#include <memory>

class DataCollector {
public:
	DataCollector(std::shared_ptr <Database> db);
	int read_values();
	int init_slave(bsc_xfer_t& xfer, int addr);   
	int close_slave(bsc_xfer_t& xfer);
private:
	void handle_message(char msgBuff[], int sz);		
	std::shared_ptr<Database> _db;
};


#endif /* data_collection.hpp */
