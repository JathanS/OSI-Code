hw1: hw1.o Log.o SetupData.o
	g++ -std=c++11 hw1.o Log.o SetupData.o -o hw1
hw1.o: hw1.cc Log.h SetupData.h
	g++ -std=c++11 -c -g hw1.cc
Log.o: Log.cc Log.h
	g++ -std=c++11 -c -g Log.cc
SetupData.o: SetupData.cc SetupData.h
	g++ -std=c++11 -c -g SetupData.cc

clean:
	rm *.o
	rm hw1
