OBJS	= main.o
SOURCE	= main.cpp 
OUT	= main
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

compilador_ci.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

clean:
	rm -f $(OBJS) $(OUT)