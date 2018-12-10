//
// data_collection.cpp
//
// Jonas Åsnes Sagild 07.12.18
//
// g++ -std=c++11 -lpigpio -lrt -o data_collection database.hpp database.cpp data_collection.cpp data_collection.hpp
#include "data_collection.hpp"

#define SLAVE_ADDR 0


DataCollector::DataCollector(std::shared_ptr <Database> db) : _db(db){

}

int DataCollector::read_values () {

	int status, j = 0;

	if (gpioInitialise() < 0 ){
		printf("Error 1 \n");
		return 1;
	}
	bsc_xfer_t xfer;
	
	status = init_slave(xfer, SLAVE_ADDR);
	while (1) {
		xfer.txCnt = 0;
		status = bscXfer(&xfer);
		if(xfer.rxCnt != 0) {
			printf("%.*s", xfer.rxCnt, xfer.rxBuf);
			this->handle_message(xfer.rxBuf, xfer.rxCnt);	
			
		printf("\n");
		}

	}
	status = close_slave(xfer);
	gpioTerminate();

}

void DataCollector::handle_message(char msgBuff[], int sz){
	// processes the messages received on the i2c

	// TODO: save the data based on what is received
	if(sz > 0) {
		_db->mux_illuminance.lock();
		_db->illuminance = _db->illuminance + 1;
		_db->mux_illuminance.unlock();	
	}

}



int DataCollector::init_slave(bsc_xfer_t& xfer, int addr) {
	gpioSetMode(18, PI_ALT3);
	gpioSetMode(19, PI_ALT3);
	xfer.control  = (addr<<16) | /* slave address*/
	(0x00<<13) | /* invert transmit status flag */
	(0x00<<12) | /* enable host control */
	(0x00<<11) | /* enable test fifo*/
	(0x00<<10) | /* invert receive status flag */
	(0x01<<9) | /* enable receive */
	(0x01<<8) | /* enable transmit */
	(0x00<<7) | /* abort and clear FIFOs */
	(0x00<<6) | /* send control reg as 1st i2c byte */
	(0x00<<5) | /* send status regr as 1st i2c byte */
	(0x00<<4) | /* set SPI polarity hight */
	(0x00<<3) | /* set SPI phase high */
	(0x01<<2) | /* enable i2c mode */
	(0x00<<1) | /* enable SPI mode */
	0x01; /* enable BSC peripheral */
	return bscXfer(&xfer);
}

int DataCollector::close_slave(bsc_xfer_t& xfer) {
	xfer.control = 0;
	return bscXfer(&xfer);
}
