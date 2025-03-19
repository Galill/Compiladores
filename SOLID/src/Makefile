OBJS	= main.o analisa.o ast_builder.o interpreter.o utils.o validator.o
SOURCE	= main.cpp utils.cpp interpreter.cpp ast_builder.cpp analisa.cpp validator.cpp
HEADER	= analisa.h ast_builder.h dicionario.h interpreter.h token.h utils.h validator.h
OUT	= compilar
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

analisa.o: analisa.cpp
	$(CC) $(FLAGS) analisa.cpp 

ast_builder.o: ast_builder.cpp
	$(CC) $(FLAGS) ast_builder.cpp 

utils.o: utils.cpp
	$(CC) $(FLAGS) utils.cpp 

interpreter.o: interpreter.cpp
	$(CC) $(FLAGS) interpreter.cpp

clean:
	rm -f $(OBJS) 