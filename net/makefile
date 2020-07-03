CPPFLAGS = -g -Wall -lpthread -std=c++11 

ALL:test

test :./*.cpp
	g++ $^ $(CPPFLAGS) -o $@

.PHONY:ALL CLEAN

clean:
	rm test
