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
//			printf("%.*s", xfer.rxCnt, xfer.rxBuf);
			this->handle_message(xfer.rxBuf, xfer.rxCnt);	
			
		}

	}
	status = close_slave(xfer);
	gpioTerminate();

}

void DataCollector::handle_message(char msgBuff[], int sz){
	// processes the messages received on the i2c
	// TODO: save the data based on what is received

	// TODO: make switch that checks what is beeing sent, and then saves the values to the database
	
	switch(msgBuff[1]) {
		case 'D':
			if (msgBuff[0] == '1') {
				// TODO: save the next values to database
				std::string streng(msgBuff, sz);
				_db->mux_dim_1.lock();
				_db->dim_1 = streng.substr(2,sizeof(msgBuff));
				_db->mux_dim_1.unlock();
			} else if (msgBuff[0] == '2') {
				std::string streng(msgBuff, sz);
				_db->mux_dim_2.lock();
				_db->dim_2 = streng.substr(2,sizeof(msgBuff));
				_db->mux_dim_2.unlock();
			}
			break;
		case 'l':
			if (msgBuff[0] == '1') {
				std::string streng(msgBuff, sz);
				_db->mux_illuminance_1.lock();
				_db->illuminance_1 = std::stoi(streng.substr(2,sizeof(msgBuff)));
				_db->mux_illuminance_1.unlock();
			} else if (msgBuff[0] == '2') {
				std::string streng(msgBuff, sz);
				_db->mux_illuminance_2.lock();
				_db->illuminance_2 = std::stoi(streng.substr(2, sizeof(msgBuff)));
				_db->mux_illuminance_2.unlock();	
			}
			break;
		case 'L':
			if (msgBuff[0] == '1') {
				std::string streng(msgBuff, sz);
				_db->mux_low_bound_1.lock();
				_db->low_bound_1 = streng.substr(2, sizeof(msgBuff));
				_db->mux_low_bound_1.unlock();
			} else if (msgBuff[0] == '2') {
				std::string streng(msgBuff, sz);
				_db->mux_low_bound_2.lock();
				_db->low_bound_2 = streng.substr(2, sizeof(msgBuff));
				_db->mux_low_bound_2.unlock();
			}
			break;
		case 'd':
			if (msgBuff[0] == '1') {
				std::string streng(msgBuff, sz);
				_db->mux_pwm_1.lock();
				_db->pwm_1 = streng.substr(2, sizeof(msgBuff));
				_db->mux_pwm_1.unlock();
			} else if (msgBuff[0] == '2') {
				std::string streng(msgBuff, sz);
				_db->mux_pwm_2.lock();
				_db->pwm_2 = streng.substr(2, sizeof(msgBuff));
				_db->mux_pwm_2.unlock();
			}
			break;
	}	
	
	
	if (msgBuff[0] == '1') {

	} else if (msgBuff[0] == '2') {

	}

	if(sz > 0) {
		_db->mux_illuminance_2.lock();
		_db->illuminance_2 = _db->illuminance_2 + 1;
		_db->mux_illuminance_2.unlock();	
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
