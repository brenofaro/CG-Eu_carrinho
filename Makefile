# Nome do arquivo fonte principal
SRC = eu-carrinho.cpp

# Nome do programa (executável) final
PROG = eu-carrinho

# Diretórios das bibliotecas adicionais, se necessário
LIB_DIRS = -Llib

# Inclusão de diretórios adicionais que contêm arquivos de cabeçalho
INCLUDE_DIRS = -Iinclude

# Bibliotecas a serem linkadas
LIBS = -lopengl32 -lglu32 -lfreeglut

# Compilador
CC = g++

# Opções de compilação
CFLAGS = -Wall $(INCLUDE_DIRS)

# Comando padrão que será executado ao rodar 'make' sem argumentos
all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(PROG) $(LIB_DIRS) $(LIBS)

# Comando para limpar os arquivos compilados, deixando apenas o código fonte
clean:
	del $(PROG).exe

# Comando para executar o programa
run: $(PROG)
	./$(PROG)

# Impede que make interprete 'clean' e 'run' como nomes de arquivos
.PHONY: clean run
