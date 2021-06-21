FLAGS = -fsanitize=address -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format

all: server client

server:
%.o: server%.o
	g++ $(FLAGS) $< -o -O3 $@
%.o: %.cpp 
	g++ -c $(FLAGS) $< -o $@

server.o: server.cpp
record.o: record.cpp record.h
command.o: command.cpp command.h
command_type.o: command_type.cpp command_type.h
condition.o: condition.cpp condition.h
operation.o: operation.cpp operation.h
ordering.o: ordering.cpp ordering.h
list.o: list.cpp list.h
avl_tree.o: avl_tree.cpp avl_tree.h
b_tree.o: b_tree.cpp b_tree.h
structure.o: structure.cpp structure.h

server: server.o command.o command_type.o record.o condition.o operation.o ordering.o list.o avl_tree.o b_tree.o structure.o
	g++ server.o command.o command_type.o record.o condition.o operation.o ordering.o list.o avl_tree.o b_tree.o structure.o -lasan -o server

client: client.o
	g++ -Wall -O3  -g client.o -lasan -o client
client.o: client.cpp
	g++ -c $(FLAGS) -O3 client.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.exe
