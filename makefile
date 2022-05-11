SOURCES=gui.cpp main.cpp car.cpp factory.cpp

full:
	echo "Compiling..."
	g++ -o fullRace $(SOURCES) -pthread -lncurses

gui:
	echo "Compiling the race track..."
	g++ -o race ./gui.cpp -lncurses

warn-gui: 
	g++ -o race ./gui.cpp -Wall -lncurses

test:
	echo "Compiling test..."
	g++ -o testRace $(SOURCES) -pthread -lncurses

.SILENT: