#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para contar o número de linhas no arquivo CSV
int contarLinhas(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    int linhas = 0;
    char linha[512];

    // Contar as linhas
    while (fgets(linha, sizeof(linha), arquivo)) {
        linhas++;
    }

    fclose(arquivo);
    return linhas - 1; // Subtrair 1 para ignorar o cabeçalho
}

// Função para ler os dados do arquivo CSV
int lerDados(const char *nomeArquivo, Processo *processos) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    char linha[512];
    int i = 0;

    // Ignorar o cabeçalho
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        // Remove o '\n' no final da linha, se existir
        linha[strcspn(linha, "\n")] = '\0';

        // Array para armazenar os campos extraídos
        char campos[6][256] = { {0} };
        int campoIndex = 0;
        char *ptr = linha;

        while (*ptr && campoIndex < 6) {
            // Se o campo começa com aspas, precisamos buscar o fechamento, que pode incluir vírgulas
            if (*ptr == '"') {
                ptr++;
                char *start = ptr;
                while (*ptr && *ptr != '"') {
                    ptr++;
                }
                int len = ptr - start;
                strncpy(campos[campoIndex], start, len);
                campos[campoIndex][len] = '\0';
                if (*ptr == '"') {
                    ptr++;
                }
                if (*ptr == ',') {
                    ptr++;
                }
            } else { 
                // Campo sem aspas (formato simples, sem vírgulas internas)
                char *start = ptr;
                while (*ptr && *ptr != ',') {
                    ptr++;
                }
                int len = ptr - start;
                strncpy(campos[campoIndex], start, len);
                campos[campoIndex][len] = '\0';
                if (*ptr == ',') {
                    ptr++;
                }
            }
            campoIndex++;
        }

        // Ppopula a estrutura Processo usando os campos extraídos
        processos[i].id = atoi(campos[0]);

        // Campo numero já foi processado, não incluir aspas adicionais
        strncpy(processos[i].numero, campos[1], sizeof(processos[i].numero));

        // Campo data_ajuizamento
        strncpy(processos[i].data_ajuizamento, campos[2], sizeof(processos[i].data_ajuizamento));

        // Campo id_classe:
        if (campos[3][0] != '{') {
            snprintf(processos[i].id_classe, sizeof(processos[i].id_classe), "{%s}", campos[3]);
        } else {
            strncpy(processos[i].id_classe, campos[3], sizeof(processos[i].id_classe));
        }

        // Campo id_assunto:
        if (campos[4][0] != '{') {
            snprintf(processos[i].id_assunto, sizeof(processos[i].id_assunto), "{%s}", campos[4]);
        } else {
            strncpy(processos[i].id_assunto, campos[4], sizeof(processos[i].id_assunto));
        }

        processos[i].ano_eleicao = atoi(campos[5]);
        i++;
    }

    fclose(arquivo);
    return i;
}

void quicksort(Processo processos[], int low, int high) {
    if (low < high) {
        // Particionar o array e obter o índice do pivô
        int pivotIndex = partition(processos, low, high);

        // Ordenar as duas metades recursivamente
        quicksort(processos, low, pivotIndex - 1);
        quicksort(processos, pivotIndex + 1, high);
    }
}

int partition(Processo processos[], int low, int high) {
    // Escolher o último elemento como pivô
    int pivot = processos[high].id;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (processos[j].id <= pivot) {
            i++;
            // Trocar processos[i] e processos[j]
            Processo temp = processos[i];
            processos[i] = processos[j];
            processos[j] = temp;
        }
    }

    // Colocar o pivô na posição correta
    Processo temp = processos[i + 1];
    processos[i + 1] = processos[high];
    processos[high] = temp;

    return i + 1;
}

// Função para ordenar os processos pelo atributo id
void ordenarPorId(Processo processos[], int n) {
    quicksort(processos, 0, n - 1);
}

// Função para salvar os dados ordenados em um arquivo CSV
void salvarDadosOrdenados(const char *nomeArquivo, Processo processos[], int n) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        perror("Erro ao criar o arquivo");
        return;
    }

    // Escrever o cabeçalho (ajuste os delimitadores conforme necessário)
    fprintf(arquivo, "id;numero;data_ajuizamento;id_classe;id_assunto;ano_eleicao\n");

    for (int i = 0; i < n; i++) {
        // Imprime o id, numero e data_ajuizamento como de costume.
        fprintf(arquivo, "%d,\"%s\",%s,", 
                processos[i].id,
                processos[i].numero,
                processos[i].data_ajuizamento);

        // Campo id_classe:
        if (strchr(processos[i].id_classe, ',') != NULL) {
            fprintf(arquivo, "\"%s\",", processos[i].id_classe);
        } else {
            fprintf(arquivo, "%s,", processos[i].id_classe);
        }

        // Campo id_assunto:
        if (strchr(processos[i].id_assunto, ',') != NULL) {
            fprintf(arquivo, "\"%s\",", processos[i].id_assunto);
        } else {
            fprintf(arquivo, "%s,", processos[i].id_assunto);
        }

        // Imprime o ano da eleição
        fprintf(arquivo, "%d\n", processos[i].ano_eleicao);
    }

    fclose(arquivo);
}