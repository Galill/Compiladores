# Variáveis
CC = g++
CXXFLAGS = -g -Wall -w  # Flags de compilação
LFLAGS =              # Flags de link (atualmente vazias)
OUT = compilador      # Nome do executável

# Arquivos fonte, objetos e cabeçalhos
SOURCE = main.cpp utils.cpp interpreter.cpp ast_builder.cpp analisa.cpp validator.cpp gerar_codigo.cpp
OBJS = $(SOURCE:.cpp=.o)
HEADER = analisa.h ast_builder.h dicionario.h interpreter.h token.h utils.h validator.h gerar_codigo.h

# Regra padrão (chama tudo)
all: $(OUT)

# Regra para criar o executável
$(OUT): $(OBJS)
	$(CC) $(OBJS) -o $(OUT) $(LFLAGS)

# Regra para compilar arquivos .cpp em .o (sem repetição)
%.o: %.cpp $(HEADER)
	$(CC) $(CXXFLAGS) -c $< -o $@

# Limpar arquivos objetos
clean:
	rm -f $(OBJS)

# Incluir dependências automáticas (caso haja cabeçalhos)
-include $(OBJS:.o=.d)
