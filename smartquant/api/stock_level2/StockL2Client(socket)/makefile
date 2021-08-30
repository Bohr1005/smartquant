CXX=/usr/bin/g++ -std=c++11
TARGET=StockL2Client

CPPS += StockL2Client.cpp

INC += -I.

LINK_PATH += -L.

RUN_PATH += -Wl,-rpath,.

all:
	@echo " `date`"
	$(CXX) -o $(TARGET) $(INC) $(CPPS) $(LINK_PATH) $(RUN_PATH)
	
debug:
	@echo " `date`"
	$(CXX) -o $(TARGET) $(INC) $(CPPS) $(LINK_PATH) $(RUN_PATH) -gdwarf-2

clean :
	rm -rf *.o $(TARGET) *.con *.a Debug Release

clc :
	rm -rf *.o $(TARGET) *.con *.a Debug Release *.log *.tick *.mmap

