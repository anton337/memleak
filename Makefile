TARGET = ldetect

all:dbg_defs.o dbg_map.o
	g++ -g3 -O0 -rdynamic -D__DBG__ main.cpp -o $(TARGET) dbg_defs.o dbg_map.o

dbg_defs.o:
	g++ -Werror -c -g3 -O0 -rdynamic -D__DBG__ dbg_defs.cpp -o dbg_defs.o

dbg_map.o:
	g++ -Werror -c -g3 -O0 -rdynamic -D__DBG__ dbg_map.cpp -o dbg_map.o

clean:
	rm -f $(TARGET) *.o

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind-log ./$(TARGET)

