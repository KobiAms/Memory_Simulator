all: main.cpp sim_mem.h sim_mem.cpp
	g++ main.cpp sim_mem.cpp -o sim_mem
all-GDB: main.cpp sim_mem.h sim_mem.cpp
	g++ -g main.cpp sim_mem.cpp -o sim_mem
