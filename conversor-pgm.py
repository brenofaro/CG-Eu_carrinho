def ler_escrever_pgm(arquivo_entrada, arquivo_saida):
    with open(arquivo_entrada, 'r') as file:
        # Ler o cabeçalho do arquivo PGM
        tipo = file.readline().strip()
        # Ignorar comentários
        while True:
            proxima_linha = file.readline().strip()
            if proxima_linha[0] != '#':
                dimensoes = proxima_linha
                break
        max_valor = file.readline().strip()

        # Ler os dados da imagem
        dados = file.read().split()

    # Converter os dados para inteiros
    dados_int = [int(i) for i in dados]

    # Escrever o novo arquivo PGM
    with open(arquivo_saida, 'w') as file:
        file.write(f'{tipo}\n')
        file.write(f'{dimensoes}\n')
        file.write(f'{max_valor}\n')

        # Distribuir os valores uniformemente
        largura, altura = map(int, dimensoes.split())
        for i in range(altura):
            linha = ' '.join(str(dados_int[i * largura + j]) if (i * largura + j) < len(dados_int) else '0'
                             for j in range(largura)) + '\n'
            file.write(linha)

# Exemplo de uso:
ler_escrever_pgm('teste.pgm', 'saida.pgm')
