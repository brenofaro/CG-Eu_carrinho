# Nome do arquivo fonte principal
SRC = eu-carrinho.cpp

# Nome do programa (executável) final
PROG = eu-carrinho

# Detecta o sistema operacional
ifeq ($(OS),Windows_NT)
    detected_OS := Windows
    RM = del /Q
    FixPath = $(subst /,\,$1)
    EXECUTABLE := $(PROG).exe
else
    detected_OS := $(shell uname -s)
    RM = rm -f
    FixPath = $1
    EXECUTABLE := $(PROG)
endif

# Diretórios das bibliotecas adicionais, se necessário
LIB_DIRS = -Llib

# Inclusão de diretórios adicionais que contêm arquivos de cabeçalho
INCLUDE_DIRS = -Iinclude

# Bibliotecas a serem linkadas
ifeq ($(detected_OS),Windows)
    LIBS = -lopengl32 -lglu32 -lfreeglut
else
    LIBS = -lGL -lGLU -lglut
endif

# Compilador
CC = g++

# Opções de compilação
CFLAGS = -Wall $(INCLUDE_DIRS)

# Comando padrão que será executado ao rodar 'make' sem argumentos
all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXECUTABLE) $(LIB_DIRS) $(LIBS)

# Comando para limpar os arquivos compilados, deixando apenas o código fonte
clean:
	$(RM) $(call FixPath,$(EXECUTABLE))

# Comando para executar o programa
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Impede que make interprete 'clean' e 'run' como nomes de arquivos
.PHONY: clean run
