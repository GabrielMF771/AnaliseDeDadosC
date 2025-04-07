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
    char id_classe_temp[50];
    char id_assunto_temp[50];
    int i = 0;

    // Ignorar o cabeçalho
    fgets(linha, sizeof(linha), arquivo);

    while (fgets(linha, sizeof(linha), arquivo)) {
        processos[i].numero[0] = '\0';
        processos[i].data_ajuizamento[0] = '\0';
        processos[i].id_classe[0] = '\0';
        processos[i].id_assunto[0] = '\0';
        processos[i].ano_eleicao = 0;

        // Verificar formato dos campos id_classe e id_assunto
        if (strstr(linha, "\"{") != NULL) {
            // Tem pelo menos um campo multivalorado
            sscanf(linha, "%d,\"%49[^\"]\",%49[^,],\"%49[^\"]\",\"%49[^\"]\",%d",
                   &processos[i].id,
                   processos[i].numero,
                   processos[i].data_ajuizamento,
                   id_classe_temp,
                   id_assunto_temp,
                   &processos[i].ano_eleicao);
        } else {
            // Formato simples para ambos os campos
            sscanf(linha, "%d,\"%49[^\"]\",%49[^,],{%49[^}]},{%49[^}]},%d",
                   &processos[i].id,
                   processos[i].numero,
                   processos[i].data_ajuizamento,
                   id_classe_temp,
                   id_assunto_temp,
                   &processos[i].ano_eleicao);
        }

        // Copiar o valor de id_classe com o formato apropriado
        if (strstr(id_classe_temp, ",") != NULL) {
            snprintf(processos[i].id_classe, sizeof(processos[i].id_classe), "\"%s\"", id_classe_temp);
        } else {
            snprintf(processos[i].id_classe, sizeof(processos[i].id_classe), "{%s}", id_classe_temp);
        }

        // Copiar o valor de id_assunto com o formato apropriado
        if (strstr(id_assunto_temp, ",") != NULL) {
            snprintf(processos[i].id_assunto, sizeof(processos[i].id_assunto), "\"%s\"", id_assunto_temp);
        } else {
            snprintf(processos[i].id_assunto, sizeof(processos[i].id_assunto), "{%s}", id_assunto_temp);
        }

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

    // Escrever o cabeçalho
    fprintf(arquivo, "id;numero;data_ajuizamento;id_classe;id_assunto;ano_eleicao\n");

    // Escrever os dados
    for (int i = 0; i < n; i++) {
        fprintf(arquivo, "%d,\"%s\",%s,%s,%s,%d\n",
                processos[i].id,
                processos[i].numero[0] ? processos[i].numero : "",
                processos[i].data_ajuizamento[0] ? processos[i].data_ajuizamento : "",
                processos[i].id_classe[0] ? processos[i].id_classe : "",
                processos[i].id_assunto[0] ? processos[i].id_assunto : "",
                processos[i].ano_eleicao);
    }

    fclose(arquivo);
}