# 
# makefile
#
# Jonas Åsnes Sagild 12.12.18
#


# compiler
CXX = g++

# Compiler flags
CPPFLAGS = -std=c++11
CFLAGS = -lpthread -lboost_system -lpigpio -lrt


# sources
SOURCES = database.hpp database.cpp data_collection.hpp data_collection.cpp session.hpp session.cpp server.cpp server.hpp system.cpp system.hpp

system: $(SOURCES)
	$(CXX) $(CPPFLAGS) $(SOURCES) $(CFLAGS) -o system
