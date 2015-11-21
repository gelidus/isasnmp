CXX = g++
CFLAGS = -std=c++11 -Wall -g
SOURCES = SNMPPacket.cpp InterfaceInfoContainer.cpp SNMPClient.cpp types.cpp main.cpp
EXECUTABLE = isaSnmpIfLog
OBJECTS = $(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@

