CXX=/usr/bin/g++ -std=c++11
TARGET=StockL2Client

CPPS += StockL2Client.cpp

INC += -I.
INC += -I/mnt/openonload-201811-u1/src/include

LIBS += -lciul

LINK_PATH += -L.
LINK_PATH += -L/mnt/openonload-201811-u1/src/include
LINK_PATH += -L/mnt/openonload-201811-u1/build/gnu_x86_64/lib/ciul
LINK_PATH += -L/mnt/openonload-201811-u1/build/gnu_x86_64/lib/citools

RUN_PATH += -Wl,-rpath,.
RUN_PATH += -Wl,-rpath,/usr/local/lib
RUN_PATH += -Wl,-rpath,/mnt/openonload-201811-u1/src/include
RUN_PATH += -Wl,-rpath,/mnt/openonload-201811-u1/build/gnu_x86_64/lib/ciul
RUN_PATH += -Wl,-rpath,/mnt/openonload-201811-u1/build/gnu_x86_64/lib/citools

all:
	@echo " `date`"
	$(CXX) -o $(TARGET) $(INC) $(CPPS) $(LINK_PATH) $(LIBS) $(RUN_PATH)
	
debug:
	@echo " `date`"
	$(CXX) -o $(TARGET) $(INC) $(CPPS) $(LINK_PATH) $(LIBS) $(RUN_PATH) -gdwarf-2

clean :
	rm -rf *.o $(TARGET) *.con *.a Debug Release

clc :
	rm -rf *.o $(TARGET) *.con *.a Debug Release *.log *.tick *.mmap

