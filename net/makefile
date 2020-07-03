CPPFLAGS = -g -Wall -lpthread -std=c++11 

ALL:test_eventloop

test_eventloop :./*.cpp
	g++ $^ $(CPPFLAGS) -o $@

.PHONY:ALL CLEAN

clean:
	rm test_eventloop
