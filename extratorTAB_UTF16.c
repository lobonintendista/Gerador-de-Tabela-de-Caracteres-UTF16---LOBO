#include <stdio.h>
#include <ctype.h>  // Para a função isprint
#include <string.h>
#include <locale.h>

void exibirCabecalho() {
    printf("\n     ==================================================\n");
    printf("        Gerador de Tabela de Caracteres UTF16 - LOBO\n");
    printf("     ==================================================\n\n");
}

int processarArquivo(const char *binFilename) {
    unsigned char tabela[256];  // Tabela a ser gerada
    char txtFilename[256]; // Nome do arquivo de saída
    FILE *binFile;
    FILE *txtFile; // Ponteiro para o arquivo de saída
    unsigned char byteLido;
    unsigned long offsetEntrada, offsetSaida;  // Usar unsigned long para acomodar offsets grandes

    // Inicializando a tabela com '@' para os valores não preenchidos
    for (int i = 0; i < 256; i++) {
        tabela[i] = '@';
    }

    // Abrir o arquivo binário para leitura
    binFile = fopen(binFilename, "rb");
    if (binFile == NULL) {
        printf("Erro: Não foi possível abrir o arquivo '%s'.\n", binFilename);
        return 1;
    }

    // Pedir os offsets de entrada e saída
    printf("  -  Digite o offset inicial de entrada (em hexadecimal): ");
    scanf("%lx", &offsetEntrada);
    printf("  -  Digite o offset de saída (em hexadecimal): ");
    scanf("%lx", &offsetSaida);

    // Verificar se os offsets são válidos
    if (offsetEntrada < 0 || offsetSaida < offsetEntrada) {
        printf("Erro: Offsets inválidos. Verifique os valores fornecidos.\n");
        fclose(binFile);
        return 1;
    }

    // Posicionar o ponteiro do arquivo no offset de entrada
    fseek(binFile, offsetEntrada, SEEK_SET);

    // Ler o arquivo binário byte a byte até o offset de saída
    for (unsigned long offset = offsetEntrada; offset <= offsetSaida; offset++) {
        // Lê um byte do arquivo
        if (fread(&byteLido, 1, 1, binFile) == 1) {
            // Verifica se o byte lido é imprimível
            if (isprint(byteLido)) {
                tabela[byteLido] = byteLido;  // Mapeia o byte para o caractere correspondente
            }
        } else {
            break; // Para se não conseguir ler mais bytes
        }
    }

    fclose(binFile);

    // Construir o nome do arquivo de saída baseado no arquivo binário
    snprintf(txtFilename, sizeof(txtFilename), "%s_tabela.tbl", binFilename);

    // Abrir o arquivo de saída para gravação
    txtFile = fopen(txtFilename, "w");
    if (txtFile == NULL) {
        printf("Erro: Não foi possível criar o arquivo '%s'.\n", txtFilename);
        return 1;
    }

    // Escrever a tabela gerada no arquivo de saída
    for (int i = 0; i < 256; i++) {
        if (tabela[i] != '@') {
            fprintf(txtFile, "00%02X=%c\n", i, tabela[i]); // Adicionando "00" antes do valor
        } else {
            fprintf(txtFile, "00%02X=[%02X]\n", i, i); // Quando não há um caractere imprimível
        }
    }

    fclose(txtFile);

    printf("\nTabela de caracteres gerada e salva em '%s'.\n", txtFilename);
    return 0;
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    char binFilename[256];
    char opcao;

    // Exibir cabeçalho
    exibirCabecalho();

    do {
        // Pedir o nome do arquivo binário
        printf("  -  Digite o nome do arquivo binário (com extensão): ");
        scanf("%s", binFilename);

        // Processar o arquivo
        if (processarArquivo(binFilename) != 0) {
            printf("Erro ao processar o arquivo '%s'. Tente novamente.\n", binFilename);
        }

        // Perguntar se deseja processar outro arquivo
        printf("\n  -  Deseja processar outro arquivo? (s/n): ");
        scanf(" %c", &opcao);  // Espaço antes de %c para ignorar quebra de linha pendente

    } while (opcao == 's' || opcao == 'S');

    printf("\n=================================\n");
    printf("         Fim do Programa         \n");
    printf("=================================\n\n");
    printf("             by LOBO             \n\n");

    return 0;
}
