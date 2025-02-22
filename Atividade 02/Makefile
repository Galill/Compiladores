OBJS	= compilador_ci.o
SOURCE	= compilador_ci.cpp 
OUT	= compilador
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

compilador_ci.o: compilador_ci.cpp
	$(CC) $(FLAGS) compilador_ci.cpp 

clean:
	rm -f $(OBJS) $(OUT)