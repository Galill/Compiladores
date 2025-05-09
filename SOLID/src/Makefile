
CC = g++
CXXFLAGS = -g  -Wall -w
LFLAGS =              
OUT = compilador      


SOURCE = main.cpp utils.cpp interpreter.cpp ast_builder.cpp analisa.cpp validator.cpp gerar_codigo.cpp
OBJS = $(SOURCE:.cpp=.o)
HEADER = analisa.h ast_builder.h dicionario.h interpreter.h token.h utils.h validator.h gerar_codigo.h


all: $(OUT)


$(OUT): $(OBJS)
	$(CC) $(OBJS) -o $(OUT) $(LFLAGS)


%.o: %.cpp $(HEADER)
	$(CC) $(CXXFLAGS) -c $< -o $@


clean:
	rm -f $(OBJS)


-include $(OBJS:.o=.d)
